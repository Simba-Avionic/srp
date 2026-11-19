/**
 * Copyright (c) 2024
 *
 * @file e220_radio_driver.cpp
 */
#include "core/radio/e220/e220_radio_driver.hpp"

#include <algorithm>
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
using ara::com::mavlink::TransportErrc;

constexpr std::uint8_t kCmdWrite = 0xC0;
constexpr std::uint8_t kCmdRead = 0xC1;
constexpr std::uint32_t kConfigBaud = 9600;  // 6.1 (p. 30): configuration only at 9600 8N1
constexpr std::size_t kRxChunk = 512;

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
}  // namespace

E220RadioDriver::E220RadioDriver(std::string name, std::string uart_port, E220Config config,
                                 std::shared_ptr<uart::IUartDriver> uart,
                                 std::shared_ptr<IE220Pins> pins, E220Timing timing)
    : name_{std::move(name)},
      uart_port_{std::move(uart_port)},
      config_{config},
      timing_{timing},
      uart_{std::move(uart)},
      pins_{std::move(pins)} {}

std::shared_ptr<E220RadioDriver> E220RadioDriver::WithConfigError(std::string name, std::string config_error) {
  auto d = std::make_shared<E220RadioDriver>(std::move(name), "", E220Config{}, nullptr, nullptr);
  d->config_error_ = std::move(config_error);
  return d;
}

E220RadioDriver::~E220RadioDriver() { Stop(); }

Registers E220RadioDriver::last_registers() const {
  std::lock_guard<std::mutex> lock{regs_mutex_};
  return last_registers_;
}

bool E220RadioDriver::SleepFor(std::chrono::milliseconds d, const std::stop_token& token) {
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

bool E220RadioDriver::WaitAux(bool idle, std::chrono::milliseconds timeout, const std::stop_token& token) {
  const auto end = Clock::now() + timeout;
  for (;;) {
    if (token.stop_requested()) {
      return false;
    }
    bool high = false;
    if (pins_->ReadAux(&high) && (high == config_.aux_idle_level) == idle) {
      return true;
    }
    if (Clock::now() >= end) {
      return false;
    }
    std::this_thread::sleep_for(timing_.aux_poll);
  }
}

bool E220RadioDriver::SwitchMode(bool config_mode, const std::stop_token& token, std::string* error) {
  // AUX must be idle before a mode change (5.2.4, p. 27-28)...
  if (!WaitAux(true, timing_.aux_timeout, token)) {
    *error = "AUX is not idle before the mode change (timeout " +
             std::to_string(timing_.aux_timeout.count()) + " ms)";
    return false;
  }
  if (!pins_->SetMode(config_mode, config_mode)) {
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
      if (pins_->ReadAux(&high) && high != config_.aux_idle_level) {
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

bool E220RadioDriver::OpenUart(std::uint32_t baud, uart::Parity parity) {
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

void E220RadioDriver::FlushRx() {
  for (int i = 0; i < 64 && uart_->WaitForData(std::chrono::milliseconds(0), {}); ++i) {
    if (!uart_->Read(256)) {
      break;
    }
  }
}

std::optional<std::vector<std::uint8_t>> E220RadioDriver::Exchange(
    const std::vector<std::uint8_t>& cmd, std::size_t expected, const std::stop_token& token,
    std::string* error) {
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
      *error = "no response to " + HexBytes(cmd) + "(timeout " +
               std::to_string(timing_.command_timeout.count()) + " ms, received " +
               std::to_string(resp.size()) + " B: " + HexBytes(resp) + ")";
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

bool E220RadioDriver::ReadRegisters(Registers* regs, const std::stop_token& token, std::string* error) {
  const auto resp = Exchange({kCmdRead, 0x00, static_cast<std::uint8_t>(kRegisterCount)},
                             3 + kRegisterCount, token, error);
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

std::optional<std::string> E220RadioDriver::ConfigureOnce(const std::stop_token& token) {
  std::string err;
  if (!pins_->Prepare(&err)) {
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

// ------------------------------------------------------------------ Start / Stop

ara::core::Result<void> E220RadioDriver::Start(ara::com::mavlink::IRadioListener& listener,
                                               std::stop_token token) noexcept {
  std::lock_guard<std::mutex> life{lifecycle_mutex_};
  if (!config_error_.empty()) {
    return MakeTransportError(TransportErrc::kPersistent, Name() + ": " + config_error_);
  }
  if (rx_thread_.joinable() || tx_thread_.joinable()) {
    return MakeTransportError(TransportErrc::kInvalidArgument, Name() + ": Start() without Stop()");
  }
  const std::string invalid = Validate(config_);
  if (!invalid.empty()) {
    return MakeTransportError(TransportErrc::kPersistent, Name() + ": E220 configuration: " + invalid);
  }
  if (token.stop_requested()) {
    return MakeTransportError(TransportErrc::kClosed, Name() + ": Start interrupted");
  }
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_ = std::stop_source{};
  }
  // The token passed to Start() (daemon shutdown) also interrupts the modem configuration.
  std::stop_callback on_caller_stop{token, [this]() {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }};
  std::stop_token st;
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    st = stop_.get_token();
  }
  std::string last = "no attempts";
  bool configured = false;
  for (int attempt = 1; attempt <= timing_.attempts; ++attempt) {
    if (st.stop_requested()) {
      uart_->Close();
      return MakeTransportError(TransportErrc::kClosed, Name() + ": Start interrupted");
    }
    const auto err = ConfigureOnce(st);
    if (!err) {
      configured = true;
      break;
    }
    last = *err;
    uart_->Close();
    std::ignore = pins_->SetMode(false, false);  // the module back in normal mode
    if (attempt < timing_.attempts && !SleepFor(timing_.retry_delay, st)) {
      break;
    }
  }
  if (!configured) {
    if (st.stop_requested()) {
      return MakeTransportError(TransportErrc::kClosed, Name() + ": Start interrupted");
    }
    return MakeTransportError(TransportErrc::kRetryable,
                              Name() + ": E220 configuration failed after " +
                                  std::to_string(timing_.attempts) + " attempts: " + last);
  }
  {
    std::lock_guard<std::mutex> lock{mutex_};
    queue_.clear();
    queued_bytes_ = 0;
    tx_ready_owed_ = false;
    busy_need_ = 0;
    front_offset_ = 0;
    fatal_.store(false);
    running_ = true;
  }
  listener_ = &listener;
  try {
    rx_thread_ = std::thread{[this, t = st] { RxLoop(t); }};
    tx_thread_ = std::thread{[this, t = st] { TxLoop(t); }};
  } catch (...) {
    StopThreadsLocked();
    return MakeTransportError(TransportErrc::kRetryable, Name() + ": could not start the threads");
  }
  return {};
}

void E220RadioDriver::StopThreadsLocked() {
  {
    std::lock_guard<std::mutex> lock{mutex_};
    running_ = false;
  }
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }
  cv_.notify_all();
  if (rx_thread_.joinable()) {
    rx_thread_.join();
  }
  if (tx_thread_.joinable()) {
    tx_thread_.join();
  }
  uart_->Close();
}

void E220RadioDriver::Stop() noexcept {
  {
    // Stop() also interrupts a running Start() (the modem configuration may take seconds) before it
    // takes the lifecycle_mutex_ held by Start().
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }
  std::lock_guard<std::mutex> life{lifecycle_mutex_};
  if (!uart_) {
    return;
  }
  StopThreadsLocked();
}

// ------------------------------------------------------------------ Send (non-blocking)

ara::core::Result<void> E220RadioDriver::Send(std::span<const std::uint8_t> frame) noexcept {
  std::lock_guard<std::mutex> lock{mutex_};
  if (!running_) {
    return MakeTransportError(TransportErrc::kClosed, Name() + ": stopped");
  }
  if (fatal_.load()) {
    return MakeTransportError(TransportErrc::kPersistent, Name() + ": radio unusable (fatal error)");
  }
  if (frame.empty()) {
    return {};
  }
  if (frame.size() > kTxQueueBytes) {
    return MakeTransportError(TransportErrc::kInvalidArgument,
                              Name() + ": frame of " + std::to_string(frame.size()) + " B > " +
                                  std::to_string(kTxQueueBytes) + " B");
  }
  if (queued_bytes_ + frame.size() > kTxQueueBytes) {
    tx_ready_owed_ = true;
    busy_need_ = frame.size();
    return MakeTransportError(TransportErrc::kBusy, Name() + ": TX queue full (" +
                                                        std::to_string(kTxQueueBytes) + " B)");
  }
  queue_.emplace_back(frame.begin(), frame.end());
  queued_bytes_ += frame.size();
  cv_.notify_all();
  return {};
}

// ------------------------------------------------------------------ threads

void E220RadioDriver::Fatal(ara::core::ErrorCode e) {
  if (fatal_.exchange(true)) {
    return;  // one OnError(fatal) per failure
  }
  {
    std::lock_guard<std::mutex> lock{mutex_};
    running_ = false;  // Send() returns an error until the daemon calls Stop()/Start()
  }
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }
  cv_.notify_all();
  listener_->OnError(std::move(e), true);
}

void E220RadioDriver::RxLoop(std::stop_token token) {
  while (!token.stop_requested()) {
    if (!uart_->WaitForData(timing_.read_slice, token)) {
      continue;  // timeout or stop
    }
    const auto data = uart_->Read(static_cast<std::uint16_t>(kRxChunk));
    if (token.stop_requested()) {
      return;
    }
    if (!data) {
      Fatal(MakeTransportError(TransportErrc::kPersistent,
                               Name() + ": UART read failed (device lost?)"));
      return;
    }
    if (!data->empty()) {
      listener_->OnReceive(std::span<const std::uint8_t>(data->data(), data->size()));
    }
  }
}

bool E220RadioDriver::AuxFailed(const char* what, const std::stop_token& token) {
  if (!token.stop_requested()) {
    Fatal(MakeTransportError(TransportErrc::kPersistent,
                             Name() + ": " + what + " (timeout " + std::to_string(timing_.aux_timeout.count()) +
                                 " ms): the module does not respond"));
  }
  return false;
}

bool E220RadioDriver::WritePortion(const std::vector<std::uint8_t>& portion, const std::stop_token& token) {
  // AUX idle = the module is not receiving a frame from the UART nor switching mode.
  if (!WaitAux(true, timing_.aux_timeout, token)) {
    return AuxFailed("AUX did not become idle before the write", token);
  }
  if (token.stop_requested()) {
    return false;
  }
  if (uart_->Write(portion) != ErrorCode::kOk) {
    Fatal(MakeTransportError(TransportErrc::kPersistent, Name() + ": UART write failed"));
    return false;
  }
  std::lock_guard<std::mutex> lock{mutex_};
  queued_bytes_ -= portion.size();
  front_offset_ += portion.size();
  if (front_offset_ >= queue_.front().size()) {
    queue_.pop_front();
    front_offset_ = 0;
  }
  return true;
}

bool E220RadioDriver::AwaitModule(const std::stop_token& token) {
  // The module signals reception of the data with AUX busy and the end of its transmission with AUX
  // idle again (5.2.3); only then is the next portion allowed.
  if (!WaitAux(false, timing_.aux_timeout, token)) {
    return AuxFailed("AUX did not become busy after the write", token);
  }
  if (!WaitAux(true, timing_.aux_timeout, token)) {
    return AuxFailed("AUX did not become idle after the write", token);
  }
  return true;
}

void E220RadioDriver::TxLoop(std::stop_token token) {
  std::unique_lock<std::mutex> lock{mutex_};
  while (!token.stop_requested()) {
    if (tx_ready_owed_ && queued_bytes_ + busy_need_ <= kTxQueueBytes) {
      tx_ready_owed_ = false;
      lock.unlock();
      listener_->OnTxReady();  // outside the mutex: the listener may call Send() right away
      lock.lock();
      continue;
    }
    if (queue_.empty()) {
      cv_.wait_for(lock, std::chrono::milliseconds(100));
      continue;
    }
    const auto& front = queue_.front();
    const std::size_t n = std::min<std::size_t>(front.size() - front_offset_, config_.packet_length);
    const std::vector<std::uint8_t> portion(front.begin() + static_cast<std::ptrdiff_t>(front_offset_),
                                            front.begin() + static_cast<std::ptrdiff_t>(front_offset_ + n));
    lock.unlock();
    if (!WritePortion(portion, token)) {
      return;
    }
    lock.lock();
    const bool tx_ready = tx_ready_owed_ && queued_bytes_ + busy_need_ <= kTxQueueBytes;
    tx_ready_owed_ = tx_ready_owed_ && !tx_ready;
    lock.unlock();
    if (tx_ready) {
      listener_->OnTxReady();  // the portion has left the queue: do not wait for the air time
    }
    if (!AwaitModule(token)) {
      return;
    }
    lock.lock();
  }
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
