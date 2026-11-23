/**
 * Copyright (c) 2024
 *
 * @file e220_modem.cpp
 */
#include "core/radio/e220/e220_modem.hpp"

#include <algorithm>
#include <array>
#include <thread>  // NOLINT
#include <tuple>
#include <utility>

#include "ara/com/mavlink/transport_error.h"
#include "core/common/error_code.h"
#include "core/uart/uart_config.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
using ara::com::mavlink::MakeTransportError;
using ara::com::mavlink::RadioLinkStats;
using ara::com::mavlink::TransportErrc;

constexpr std::uint8_t kCmdWrite = 0xC0;
constexpr std::uint8_t kCmdRead = 0xC1;
constexpr std::uint32_t kConfigBaud = 9600;  // 6.1 (p. 30): configuration only at 9600 8N1
constexpr std::size_t kRxChunk = 512;
// 6.3, REG1 bit 5 (p. 33): in the transparent mode `C0 C1 C2 C3 <start 00> <length>` returns
// `C1 <start> <length> <values>`; register 0x00 = ambient noise RSSI, 0x01 = RSSI of the last packet.
constexpr std::array<std::uint8_t, 6> kStatsCommand{0xC0, 0xC1, 0xC2, 0xC3, 0x00, 0x02};
constexpr std::array<std::uint8_t, 3> kStatsReplyHeader{0xC1, 0x00, 0x02};
constexpr std::size_t kStatsReplySize = 5;

uart::Parity ToUartParity(UartParity p) {
  switch (p) {
    case UartParity::k8O1: return uart::Parity::kOdd;
    case UartParity::k8E1: return uart::Parity::kEven;
    case UartParity::k8N1: break;
  }
  return uart::Parity::kNone;
}

std::string HexBytes(const std::vector<std::uint8_t>& v) {
  static const char* d = "0123456789ABCDEF";
  std::string s;
  for (const auto b : v) {
    s += d[b >> 4];
    s += d[b & 15];
    s += ' ';
  }
  return s;
}

/// dBm of an RSSI register value (manual 6.3, p. 33: dBm = -(256 - RSSI)).
std::int16_t RssiDbm(std::uint8_t value) { return static_cast<std::int16_t>(value) - 256; }

E220Config WithAmbientRssi(E220Config c, bool on) {
  c.ambient_rssi = on;
  return c;
}
}  // namespace

E220Modem::E220Modem(std::string uart_port, E220Config config, E220Pins pins,
                     std::shared_ptr<uart::IUartDriver> uart, std::shared_ptr<gpio::IGpioDriver> gpio,
                     E220Timing timing, std::chrono::milliseconds stats_period)
    : uart_port_{std::move(uart_port)},
      config_{WithAmbientRssi(config, stats_period.count() > 0)},
      pins_{pins},
      timing_{timing},
      stats_period_{stats_period},
      caps_{config.packet_length, false, false},
      uart_{std::move(uart)},
      gpio_{std::move(gpio)},
      logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger("e220", "", ara::log::LogLevel::kInfo)} {}

std::shared_ptr<E220Modem> E220Modem::WithConfigError(std::string config_error) {
  auto m = std::make_shared<E220Modem>("", E220Config{}, E220Pins{}, nullptr, nullptr);
  m->config_error_ = std::move(config_error);
  return m;
}

E220Modem::~E220Modem() { Close(); }

Registers E220Modem::last_registers() const {
  std::lock_guard<std::mutex> lock{regs_mutex_};
  return last_registers_;
}

// ------------------------------------------------------------------ pins, AUX, modes

bool E220Modem::PreparePins(std::string* error) {
  struct PinInit {
    std::uint16_t pin;
    gpio::direction_t dir;
    const char* name;
  };
  const PinInit all[] = {{pins_.m0, gpio::direction_t::OUT, "M0"},
                         {pins_.m1, gpio::direction_t::OUT, "M1"},
                         {pins_.aux, gpio::direction_t::IN, "AUX"}};
  for (const auto& p : all) {
    // An already exported pin (a restart of the driver) makes the export fail: the direction write decides.
    if (gpio_->initializePin(p.pin, p.dir) != ErrorCode::kOk && gpio_->setDirection(p.pin, p.dir) != ErrorCode::kOk) {
      *error = std::string("cannot set up the ") + p.name + " GPIO " + std::to_string(p.pin);
      return false;
    }
  }
  return true;
}

bool E220Modem::SetMode(bool m0, bool m1) {
  return gpio_->setValue(pins_.m0, m0 ? 1 : 0) == ErrorCode::kOk &&
         gpio_->setValue(pins_.m1, m1 ? 1 : 0) == ErrorCode::kOk;
}

bool E220Modem::ReadAux(bool* high) {
  const std::uint8_t v = gpio_->getValue(pins_.aux);
  if (v > 1) {
    return false;  // a value other than 0/1 (the driver error code) must not be taken for an idle AUX
  }
  *high = v == 1;
  return true;
}

bool E220Modem::SleepFor(std::chrono::milliseconds d, const std::stop_token& token) {
  const auto end = Clock::now() + d;
  while (Clock::now() < end) {
    if (token.stop_requested()) {
      return false;
    }
    const auto left = std::chrono::duration_cast<std::chrono::milliseconds>(end - Clock::now());
    std::this_thread::sleep_for(std::min(left, std::chrono::milliseconds(5)));
  }
  return !token.stop_requested();
}

bool E220Modem::WaitAux(bool idle, std::chrono::milliseconds timeout, const std::stop_token& token) {
  const auto end = Clock::now() + timeout;
  for (;;) {
    if (token.stop_requested()) {
      return false;
    }
    bool high = false;
    if (ReadAux(&high) && (high == config_.aux_idle_level) == idle) {
      return true;
    }
    if (Clock::now() >= end) {
      return false;
    }
    std::this_thread::sleep_for(timing_.aux_poll);
  }
}

bool E220Modem::SwitchMode(bool config_mode, const std::stop_token& token, std::string* error) {
  // AUX must be idle before a mode change (5.2.4, p. 27-28)...
  if (!WaitAux(true, timing_.aux_timeout, token)) {
    *error = "AUX is not idle before the mode change (timeout " + std::to_string(timing_.aux_timeout.count()) + " ms)";
    return false;
  }
  if (!SetMode(config_mode, config_mode)) {
    *error = "setting the M0/M1 pins failed";
    return false;
  }
  // ...the module holds AUX busy for ~10 ms; if it does not drop (a fast change) we go on...
  std::ignore = WaitAux(false, timing_.mode_switch_wait, token);
  // ...and after the change we wait for AUX to be idle again.
  if (!WaitAux(true, timing_.aux_timeout, token)) {
    *error = "AUX did not return to idle after the mode change (timeout " +
             std::to_string(timing_.aux_timeout.count()) + " ms)";
    return false;
  }
  // M0 and M1 are set one after the other: the module may see an intermediate state (01/10), switch
  // to it (AUX busy for ~10 ms) and only then to the target. AUX idle is therefore reliable only when
  // it holds for `mode_settle`; a drop meanwhile = another switch in progress, we keep waiting.
  const auto deadline = Clock::now() + timing_.aux_timeout;
  for (;;) {
    const auto settle_end = Clock::now() + timing_.mode_settle;
    bool dropped = false;
    while (Clock::now() < settle_end) {
      if (token.stop_requested()) {
        return false;
      }
      bool high = false;
      if (ReadAux(&high) && high != config_.aux_idle_level) {
        dropped = true;
        break;
      }
      std::this_thread::sleep_for(timing_.aux_poll);
    }
    if (!dropped) {
      return true;
    }
    const auto left = std::max(std::chrono::duration_cast<std::chrono::milliseconds>(deadline - Clock::now()),
                               std::chrono::milliseconds(0));
    if (!WaitAux(true, left, token)) {
      *error = "AUX did not settle at idle after the mode change (timeout " +
               std::to_string(timing_.aux_timeout.count()) + " ms)";
      return false;
    }
  }
}

// ------------------------------------------------------------------ UART, commands

bool E220Modem::OpenUart(std::uint32_t baud, uart::Parity parity) {
  uart::UartConfig c;
  c.baudrate = uart::GetBaudrateConstant(baud);
  c.parity = parity;
  c.raw = true;
  c.vmin = 0;
  c.vtime = 0;  // Read() does not block; we wait with WaitForData()
  c.default_read_size = 256;
  c.fail_on_apply_error = true;
  return uart_->Open(uart_port_, c);
}

void E220Modem::FlushRx() {
  for (int i = 0; i < 64 && uart_->WaitForData(std::chrono::milliseconds(0), {}); ++i) {
    if (!uart_->Read(256)) {
      break;
    }
  }
}

std::optional<std::vector<std::uint8_t>> E220Modem::Exchange(const std::vector<std::uint8_t>& cmd, std::size_t expected,
                                                             const std::stop_token& token, std::string* error) {
  FlushRx();
  if (uart_->Write(cmd) != ErrorCode::kOk) {
    *error = "writing command " + HexBytes(cmd) + "to the UART failed";
    return std::nullopt;
  }
  std::vector<std::uint8_t> resp;
  const auto end = Clock::now() + timing_.command_timeout;
  while (resp.size() < expected) {
    if (token.stop_requested()) {
      *error = "interrupted";
      return std::nullopt;
    }
    const auto left = std::chrono::duration_cast<std::chrono::milliseconds>(end - Clock::now());
    if (left.count() <= 0) {
      *error = "no response to " + HexBytes(cmd) + "(timeout " + std::to_string(timing_.command_timeout.count()) +
               " ms, received " + std::to_string(resp.size()) + " B: " + HexBytes(resp) + ")";
      return std::nullopt;
    }
    if (uart_->WaitForData(std::min(left, timing_.read_slice), token)) {
      const auto part = uart_->Read(static_cast<std::uint16_t>(expected - resp.size() + 8));
      if (!part) {
        *error = "UART read failed";
        return std::nullopt;
      }
      resp.insert(resp.end(), part->begin(), part->end());
    }
    if (resp.size() >= 3 && resp[0] == 0xFF && resp[1] == 0xFF && resp[2] == 0xFF) {
      *error = "the module replied FF FF FF (format error) to " + HexBytes(cmd);
      return std::nullopt;
    }
  }
  resp.resize(expected);
  return resp;
}

bool E220Modem::ReadRegisters(Registers* regs, const std::stop_token& token, std::string* error) {
  const auto resp =
      Exchange({kCmdRead, 0x00, static_cast<std::uint8_t>(kRegisterCount)}, 3 + kRegisterCount, token, error);
  if (!resp) {
    return false;
  }
  if ((*resp)[0] != kCmdRead || (*resp)[1] != 0x00 || (*resp)[2] != kRegisterCount) {
    *error = "unexpected reply to the register read: " + HexBytes(*resp);
    return false;
  }
  std::copy(resp->begin() + 3, resp->end(), regs->begin());
  return true;
}

// ------------------------------------------------------------------ Configure

std::optional<std::string> E220Modem::ConfigureOnce(const std::stop_token& token) {
  std::string err;
  if (!PreparePins(&err)) {
    return err;
  }
  uart_->Close();
  if (!OpenUart(kConfigBaud, uart::Parity::kNone)) {
    return "opening " + uart_port_ + " (9600 8N1) failed";
  }
  if (!SwitchMode(true, token, &err)) {
    return err;
  }
  Registers current{};
  if (!ReadRegisters(&current, token, &err)) {
    return err;
  }
  const Registers desired = DesiredRegisters(config_, current);
  const auto diff = DifferingRegisters(current, desired, config_);
  for (const auto reg : diff) {
    const std::vector<std::uint8_t> cmd{kCmdWrite, static_cast<std::uint8_t>(reg), 0x01, desired[reg]};
    const auto resp = Exchange(cmd, 4, token, &err);
    if (!resp) {
      return err;
    }
    const std::vector<std::uint8_t> want{kCmdRead, static_cast<std::uint8_t>(reg), 0x01, desired[reg]};
    if (*resp != want) {
      return "write acknowledgement of register " + std::to_string(reg) + " does not match: " + HexBytes(*resp);
    }
  }
  if (!diff.empty()) {
    logger_.LogInfo() << "e220(" << uart_port_ << "): " << diff.size() << " register(s) written to the module";
    Registers after{};
    if (!ReadRegisters(&after, token, &err)) {
      return err;
    }
    const auto still = DifferingRegisters(after, desired, config_);
    if (!still.empty()) {
      return "verification after the write failed (register " + std::to_string(still.front()) +
             "): " + DescribeRegisters(after);
    }
    current = after;
  }
  {
    std::lock_guard<std::mutex> lock{regs_mutex_};
    last_registers_ = current;
  }
  if (!SwitchMode(false, token, &err)) {
    return err;
  }
  // Normal mode: the UART with the speed/parity of REG0; the port is reopened if they differ.
  const bool same_as_config = config_.uart_baudrate == kConfigBaud && config_.parity == UartParity::k8N1;
  if (same_as_config) {
    FlushRx();
  } else {
    uart_->Close();
    if (!OpenUart(config_.uart_baudrate, ToUartParity(config_.parity))) {
      return "reopening " + uart_port_ + " (" + std::to_string(config_.uart_baudrate) + ") failed";
    }
  }
  return std::nullopt;
}

ara::core::Result<void> E220Modem::Configure(std::stop_token stop) noexcept {
  if (!config_error_.empty()) {
    return MakeTransportError(TransportErrc::kPersistent, "e220: " + config_error_);
  }
  const std::string invalid = Validate(config_);
  if (!invalid.empty()) {
    return MakeTransportError(TransportErrc::kPersistent, "e220: configuration: " + invalid);
  }
  stats_pending_ = false;
  stats_hold_.clear();
  stats_.reset();
  stats_timeout_logged_ = false;
  next_stats_request_ = Clock::now() + stats_period_;
  std::string last = "no attempts";
  for (int attempt = 1; attempt <= timing_.attempts; ++attempt) {
    if (stop.stop_requested()) {
      uart_->Close();
      return MakeTransportError(TransportErrc::kClosed, "e220: Start interrupted");
    }
    const auto err = ConfigureOnce(stop);
    if (!err) {
      logger_.LogInfo() << "e220(" << uart_port_ << "): configured, " << DescribeRegisters(last_registers());
      return {};
    }
    last = *err;
    logger_.LogWarn() << "e220(" << uart_port_ << "): configuration attempt " << attempt << "/" << timing_.attempts
                      << " failed: " << last;
    uart_->Close();
    std::ignore = SetMode(false, false);  // the module back in normal mode
    if (attempt < timing_.attempts && !SleepFor(timing_.retry_delay, stop)) {
      break;
    }
  }
  if (stop.stop_requested()) {
    return MakeTransportError(TransportErrc::kClosed, "e220: Start interrupted");
  }
  return MakeTransportError(
      TransportErrc::kRetryable,
      "e220: configuration failed after " + std::to_string(timing_.attempts) + " attempts: " + last);
}

void E220Modem::Close() noexcept {
  if (uart_) {
    uart_->Close();
  }
}

// ------------------------------------------------------------------ Transmit

ara::core::Result<void> E220Modem::AuxFailed(const char* what, const std::stop_token& token) {
  if (token.stop_requested()) {
    return MakeTransportError(TransportErrc::kClosed, "e220: stopped");
  }
  return MakeTransportError(TransportErrc::kPersistent,
                            std::string("e220: ") + what + " (timeout " + std::to_string(timing_.aux_timeout.count()) +
                                " ms): the module does not respond");
}

ara::core::Result<void> E220Modem::WritePortion(std::span<const std::uint8_t> portion, const std::stop_token& token) {
  // AUX idle = the module is not receiving a frame from the UART nor switching mode.
  if (!WaitAux(true, timing_.aux_timeout, token)) {
    return AuxFailed("AUX did not become idle before the write", token);
  }
  if (token.stop_requested()) {
    return MakeTransportError(TransportErrc::kClosed, "e220: stopped");
  }
  if (uart_->Write(std::vector<std::uint8_t>(portion.begin(), portion.end())) != ErrorCode::kOk) {
    return MakeTransportError(TransportErrc::kPersistent, "e220: UART write failed");
  }
  return {};
}

ara::core::Result<void> E220Modem::AwaitModule(const std::stop_token& token) {
  // The module signals reception of the data with AUX busy and the end of its transmission with AUX
  // idle again (5.2.3); only then is the next portion allowed.
  if (!WaitAux(false, timing_.aux_timeout, token)) {
    return AuxFailed("AUX did not become busy after the write", token);
  }
  if (!WaitAux(true, timing_.aux_timeout, token)) {
    return AuxFailed("AUX did not become idle after the write", token);
  }
  return {};
}

ara::core::Result<void> E220Modem::Transmit(std::span<const std::uint8_t> frame, std::stop_token stop) noexcept {
  std::size_t offset = 0;
  while (offset < frame.size()) {
    const std::size_t n = std::min<std::size_t>(frame.size() - offset, config_.packet_length);
    auto w = WritePortion(frame.subspan(offset, n), stop);
    if (!w.HasValue()) {
      return w;
    }
    offset += n;
    auto a = AwaitModule(stop);
    if (!a.HasValue()) {
      return a;
    }
  }
  return {};
}

// ------------------------------------------------------------------ Receive and statistics

void E220Modem::MaybeRequestStats() {
  if (stats_period_.count() <= 0 || stats_pending_ || Clock::now() < next_stats_request_) {
    return;
  }
  // Only while the module is idle (it may be receiving a packet from the air); otherwise the next Receive() retries.
  bool high = false;
  if (!ReadAux(&high) || high != config_.aux_idle_level) {
    return;
  }
  if (uart_->Write(std::vector<std::uint8_t>(kStatsCommand.begin(), kStatsCommand.end())) != ErrorCode::kOk) {
    return;  // a real UART failure shows in Transmit()/Receive()
  }
  stats_pending_ = true;
  stats_sent_at_ = Clock::now();
  next_stats_request_ = stats_sent_at_ + stats_period_;
}

std::vector<std::uint8_t> E220Modem::FilterStatsReply(std::vector<std::uint8_t> bytes) {
  stats_hold_.insert(stats_hold_.end(), bytes.begin(), bytes.end());
  std::vector<std::uint8_t> out;
  const auto release = [&](std::size_t n) {
    out.insert(out.end(), stats_hold_.begin(), stats_hold_.begin() + static_cast<std::ptrdiff_t>(n));
    stats_hold_.erase(stats_hold_.begin(), stats_hold_.begin() + static_cast<std::ptrdiff_t>(n));
  };
  if (stats_pending_) {
    const auto it = std::search(stats_hold_.begin(), stats_hold_.end(), kStatsReplyHeader.begin(),
                                kStatsReplyHeader.end());
    if (it != stats_hold_.end()) {
      const auto pos = static_cast<std::size_t>(it - stats_hold_.begin());
      if (stats_hold_.size() >= pos + kStatsReplySize) {
        RadioLinkStats s;
        s.valid = RadioLinkStats::kRssi | RadioLinkStats::kNoiseFloor;
        s.noise_floor_dbm = RssiDbm(stats_hold_[pos + 3]);
        s.rssi_dbm = RssiDbm(stats_hold_[pos + 4]);
        stats_ = s;
        stats_pending_ = false;
        stats_hold_.erase(stats_hold_.begin() + static_cast<std::ptrdiff_t>(pos),
                          stats_hold_.begin() + static_cast<std::ptrdiff_t>(pos + kStatsReplySize));
        release(stats_hold_.size());
        return out;
      }
      release(pos);  // the reply is incomplete: hold it back from its header on
    } else {
      // Hold back a tail that may be the start of the header.
      std::size_t keep = 0;
      for (std::size_t k = std::min<std::size_t>(kStatsReplyHeader.size() - 1, stats_hold_.size()); k >= 1; --k) {
        if (std::equal(stats_hold_.end() - static_cast<std::ptrdiff_t>(k), stats_hold_.end(),
                       kStatsReplyHeader.begin())) {
          keep = k;
          break;
        }
      }
      release(stats_hold_.size() - keep);
    }
    if (Clock::now() - stats_sent_at_ > timing_.command_timeout) {
      stats_pending_ = false;  // no reply: the held bytes are data after all
      if (!stats_timeout_logged_) {
        stats_timeout_logged_ = true;
        logger_.LogWarn() << "e220(" << uart_port_
                          << "): no reply to the RSSI read command (C0 C1 C2 C3 00 02) within "
                          << timing_.command_timeout.count() << " ms; the statistics are unverified on the module";
      }
      release(stats_hold_.size());
    }
    return out;
  }
  release(stats_hold_.size());
  return out;
}

ara::core::Result<std::vector<std::uint8_t>> E220Modem::Receive(std::chrono::milliseconds timeout,
                                                                std::stop_token stop) noexcept {
  MaybeRequestStats();
  std::vector<std::uint8_t> data;
  if (uart_->WaitForData(timeout, stop) && !stop.stop_requested()) {
    const auto chunk = uart_->Read(static_cast<std::uint16_t>(kRxChunk));
    if (!chunk) {
      return MakeTransportError(TransportErrc::kPersistent, "e220: UART read failed (device lost?)");
    }
    data = *chunk;
  }
  if (stats_pending_ || !stats_hold_.empty()) {
    data = FilterStatsReply(std::move(data));
  }
  return data;
}

std::optional<RadioLinkStats> E220Modem::TakeStats() noexcept {
  auto s = stats_;
  stats_.reset();
  return s;
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
