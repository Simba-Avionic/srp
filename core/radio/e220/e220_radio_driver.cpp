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
constexpr std::uint32_t kConfigBaud = 9600;  // 6.1 (str. 30): konfiguracja tylko 9600 8N1
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

bool E220RadioDriver::WaitAux(bool level, std::chrono::milliseconds timeout, const std::stop_token& token) {
  const auto end = Clock::now() + timeout;
  for (;;) {
    if (token.stop_requested()) {
      return false;
    }
    bool high = false;
    if (pins_->ReadAux(&high) && high == level) {
      return true;
    }
    if (Clock::now() >= end) {
      return false;
    }
    std::this_thread::sleep_for(timing_.aux_poll);
  }
}

bool E220RadioDriver::SwitchMode(bool config_mode, const std::stop_token& token, std::string* error) {
  // AUX musi byc wysoki przed zmiana trybu (5.2.4, str. 27-28)...
  if (!WaitAux(true, timing_.aux_timeout, token)) {
    *error = "AUX nie jest wysoki przed zmiana trybu (timeout " +
             std::to_string(timing_.aux_timeout.count()) + " ms)";
    return false;
  }
  if (!pins_->SetMode(config_mode, config_mode)) {
    *error = "ustawienie pinow M0/M1 nie powiodlo sie";
    return false;
  }
  // ...modul opuszcza AUX na ~10 ms; jesli nie opadnie (szybka zmiana), idziemy dalej...
  std::ignore = WaitAux(false, timing_.mode_switch_wait, token);
  // ...i po zmianie trybu czekamy, az AUX znow bedzie wysoki.
  if (!WaitAux(true, timing_.aux_timeout, token)) {
    *error = "AUX nie wrocil na wysoki po zmianie trybu (timeout " +
             std::to_string(timing_.aux_timeout.count()) + " ms)";
    return false;
  }
  // M0 i M1 ustawiamy kolejno: modul moze zobaczyc stan posredni (01/10), przelaczyc sie na
  // niego (AUX nisko ~10 ms) i dopiero potem na docelowy. AUX=1 jest wiec wiarygodny dopiero, gdy
  // utrzymuje sie przez `mode_settle`; opadniecie w tym czasie = kolejne przelaczanie, czekamy dalej.
  const auto deadline = Clock::now() + timing_.aux_timeout;
  for (;;) {
    const auto settle_end = Clock::now() + timing_.mode_settle;
    bool dropped = false;
    while (Clock::now() < settle_end) {
      if (token.stop_requested()) {
        return false;
      }
      bool high = false;
      if (pins_->ReadAux(&high) && !high) {
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
      *error = "AUX nie ustabilizowal sie na wysokim po zmianie trybu (timeout " +
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
  c.vtime = 0;  // Read() nie blokuje; czekamy przez WaitForData()
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
    *error = "zapis komendy " + HexBytes(cmd) + "do UART nie powiodl sie";
    return std::nullopt;
  }
  std::vector<std::uint8_t> resp;
  const auto end = Clock::now() + timing_.command_timeout;
  while (resp.size() < expected) {
    if (token.stop_requested()) {
      *error = "przerwano";
      return std::nullopt;
    }
    const auto left = std::chrono::duration_cast<std::chrono::milliseconds>(end - Clock::now());
    if (left.count() <= 0) {
      *error = "brak odpowiedzi na " + HexBytes(cmd) + "(timeout " +
               std::to_string(timing_.command_timeout.count()) + " ms, odebrano " +
               std::to_string(resp.size()) + " B: " + HexBytes(resp) + ")";
      return std::nullopt;
    }
    if (uart_->WaitForData(std::min(left, timing_.read_slice), token)) {
      const auto part = uart_->Read(static_cast<std::uint16_t>(expected - resp.size() + 8));
      if (!part) {
        *error = "odczyt UART nie powiodl sie";
        return std::nullopt;
      }
      resp.insert(resp.end(), part->begin(), part->end());
    }
    if (resp.size() >= 3 && resp[0] == 0xFF && resp[1] == 0xFF && resp[2] == 0xFF) {
      *error = "modul odpowiedzial FF FF FF (blad formatu) na " + HexBytes(cmd);
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
    *error = "nieoczekiwana odpowiedz na odczyt rejestrow: " + HexBytes(*resp);
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
    return "otwarcie " + uart_port_ + " (9600 8N1) nie powiodlo sie";
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
      return "potwierdzenie zapisu rejestru " + std::to_string(reg) + " niezgodne: " + HexBytes(*resp);
    }
  }
  if (!diff.empty()) {
    Registers after{};
    if (!ReadRegisters(&after, token, &err)) {
      return err;
    }
    const auto still = DifferingRegisters(after, desired, config_);
    if (!still.empty()) {
      return "weryfikacja po zapisie niezgodna (rejestr " + std::to_string(still.front()) +
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
  // Tryb normalny: UART z predkoscia/parzystoscia z REG0; przy zmianie ponowne otwarcie portu.
  const bool same_as_config = config_.uart_baudrate == kConfigBaud && config_.parity == UartParity::k8N1;
  if (same_as_config) {
    FlushRx();
  } else {
    uart_->Close();
    if (!OpenUart(config_.uart_baudrate, ToUartParity(config_.parity))) {
      return "ponowne otwarcie " + uart_port_ + " (" + std::to_string(config_.uart_baudrate) +
             ") nie powiodlo sie";
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
    return MakeTransportError(TransportErrc::kInvalidArgument, Name() + ": Start() bez Stop()");
  }
  const std::string invalid = Validate(config_);
  if (!invalid.empty()) {
    return MakeTransportError(TransportErrc::kPersistent, Name() + ": konfiguracja E220: " + invalid);
  }
  if (token.stop_requested()) {
    return MakeTransportError(TransportErrc::kClosed, Name() + ": Start przerwany");
  }
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_ = std::stop_source{};
  }
  // Przerwanie przez token z Start() (zatrzymanie daemona) przerywa tez konfiguracje modemu.
  std::stop_callback on_caller_stop{token, [this]() {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }};
  std::stop_token st;
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    st = stop_.get_token();
  }
  std::string last = "brak prob";
  bool configured = false;
  for (int attempt = 1; attempt <= timing_.attempts; ++attempt) {
    if (st.stop_requested()) {
      uart_->Close();
      return MakeTransportError(TransportErrc::kClosed, Name() + ": Start przerwany");
    }
    const auto err = ConfigureOnce(st);
    if (!err) {
      configured = true;
      break;
    }
    last = *err;
    uart_->Close();
    std::ignore = pins_->SetMode(false, false);  // modul z powrotem w tryb normalny
    if (attempt < timing_.attempts && !SleepFor(timing_.retry_delay, st)) {
      break;
    }
  }
  if (!configured) {
    if (st.stop_requested()) {
      return MakeTransportError(TransportErrc::kClosed, Name() + ": Start przerwany");
    }
    return MakeTransportError(TransportErrc::kRetryable,
                              Name() + ": konfiguracja E220 nie powiodla sie po " +
                                  std::to_string(timing_.attempts) + " probach: " + last);
  }
  {
    std::lock_guard<std::mutex> lock{mutex_};
    queue_.clear();
    queued_bytes_ = 0;
    tx_ready_owed_ = false;
    busy_need_ = 0;
    model_pending_ = 0;  // modul po przelaczeniu trybu ma pusty bufor
    model_inflight_ = 0;
    model_inflight_end_ = Clock::now();
    model_last_write_ = Clock::now();
    fatal_.store(false);
    running_ = true;
  }
  listener_ = &listener;
  try {
    rx_thread_ = std::thread{[this, t = st] { RxLoop(t); }};
    tx_thread_ = std::thread{[this, t = st] { TxLoop(t); }};
  } catch (...) {
    StopThreadsLocked();
    return MakeTransportError(TransportErrc::kRetryable, Name() + ": nie udalo sie uruchomic watkow");
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
    // Stop() przerywa tez trwajacy Start() (konfiguracja modemu moze trwac sekundy), zanim wejdzie
    // w lifecycle_mutex_ trzymany przez Start().
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }
  std::lock_guard<std::mutex> life{lifecycle_mutex_};
  if (!uart_) {
    return;
  }
  StopThreadsLocked();
}

// ------------------------------------------------------------------ model bufora modulu

void E220RadioDriver::ModelAdvance(Clock::time_point now) {
  const std::size_t packet_len = config_.packet_length;
  const double air_bytes_per_s = static_cast<double>(config_.air_rate_bps) / 8.0 * timing_.air_drain_factor;
  const auto gap = std::max(timing_.module_frame_gap, std::chrono::milliseconds(3 * 10 * 1000 / static_cast<long long>(  // NOLINT
                                                         std::max<std::uint32_t>(config_.uart_baudrate, 1)) + 2));
  for (;;) {
    if (model_inflight_ > 0 && model_inflight_end_ <= now) {
      model_inflight_ = 0;  // pakiet wyszedl w powietrze: miejsce zwolnione
    }
    if (model_inflight_ > 0 || model_pending_ == 0) {
      return;
    }
    Clock::time_point trigger = model_last_write_;
    if (model_pending_ < packet_len) {
      trigger = model_last_write_ + gap;  // niepelny pakiet startuje po przerwie ramki
      if (trigger > now) {
        return;
      }
    }
    const std::size_t n = std::min(model_pending_, packet_len);
    const auto start = std::max(model_inflight_end_, trigger);
    model_pending_ -= n;
    model_inflight_ = n;
    model_inflight_end_ = start + std::chrono::duration_cast<Clock::duration>(
                                      std::chrono::duration<double>(static_cast<double>(n) / air_bytes_per_s));
  }
}

E220RadioDriver::Clock::time_point E220RadioDriver::ModelNextEvent() const {
  if (model_inflight_ > 0) {
    return model_inflight_end_;
  }
  const auto gap = std::max(timing_.module_frame_gap, std::chrono::milliseconds(3 * 10 * 1000 / static_cast<long long>(  // NOLINT
                                                         std::max<std::uint32_t>(config_.uart_baudrate, 1)) + 2));
  return model_last_write_ + gap;
}

bool E220RadioDriver::WaitTxQueueEmpty(std::chrono::milliseconds timeout) {
  std::unique_lock<std::mutex> lock{mutex_};
  return cv_.wait_for(lock, timeout, [this] { return queued_bytes_ == 0 || !running_; }) && queued_bytes_ == 0;
}

// ------------------------------------------------------------------ Send (nieblokujace)

ara::core::Result<void> E220RadioDriver::Send(std::span<const std::uint8_t> frame) noexcept {
  std::lock_guard<std::mutex> lock{mutex_};
  if (!running_) {
    return MakeTransportError(TransportErrc::kClosed, Name() + ": zatrzymany");
  }
  if (fatal_.load()) {
    return MakeTransportError(TransportErrc::kPersistent, Name() + ": radio nieuzywalne (blad fatalny)");
  }
  if (frame.empty()) {
    return {};
  }
  if (frame.size() > kModuleBuffer) {
    return MakeTransportError(TransportErrc::kInvalidArgument,
                              Name() + ": ramka " + std::to_string(frame.size()) + " B > bufor modulu 400 B");
  }
  if (!timing_.test_ignore_aux_on_write) {
    ModelAdvance(Clock::now());
    if (ModelFree() < frame.size()) {
      tx_ready_owed_ = true;
      busy_need_ = frame.size();
      cv_.notify_all();  // watek TX policzy, kiedy zwolni sie miejsce
      return MakeTransportError(TransportErrc::kBusy, Name() + ": brak miejsca w buforze modulu (400 B)");
    }
  }
  queue_.emplace_back(frame.begin(), frame.end());
  queued_bytes_ += frame.size();
  cv_.notify_all();
  return {};
}

// ------------------------------------------------------------------ watki

void E220RadioDriver::Fatal(ara::core::ErrorCode e) {
  if (fatal_.exchange(true)) {
    return;  // jeden OnError(fatal) na awarie
  }
  {
    std::lock_guard<std::mutex> lock{mutex_};
    running_ = false;  // Send() zwraca blad do czasu Stop()/Start() przez daemon
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
      continue;  // timeout albo stop
    }
    const auto data = uart_->Read(static_cast<std::uint16_t>(kRxChunk));
    if (token.stop_requested()) {
      return;
    }
    if (!data) {
      Fatal(MakeTransportError(TransportErrc::kPersistent,
                               Name() + ": odczyt UART nie powiodl sie (utrata urzadzenia?)"));
      return;
    }
    if (!data->empty()) {
      listener_->OnReceive(std::span<const std::uint8_t>(data->data(), data->size()));
    }
  }
}

bool E220RadioDriver::WriteFrame(const std::vector<std::uint8_t>& frame, const std::stop_token& token) {
  // AUX wysoki = modul nie zbiera wlasnie ramki z UART / nie przelacza trybu.
  if (!timing_.test_ignore_aux_on_write && !WaitAux(true, timing_.aux_timeout, token)) {
    if (!token.stop_requested()) {
      Fatal(MakeTransportError(TransportErrc::kPersistent,
                               Name() + ": AUX nie wrocil na wysoki (timeout " +
                                   std::to_string(timing_.aux_timeout.count()) + " ms): modul nie odpowiada"));
    }
    return false;
  }
  if (token.stop_requested()) {
    return false;
  }
  if (uart_->Write(frame) != ErrorCode::kOk) {
    Fatal(MakeTransportError(TransportErrc::kPersistent, Name() + ": zapis UART nie powiodl sie"));
    return false;
  }
  {
    std::lock_guard<std::mutex> lock{mutex_};
    model_last_write_ = Clock::now();
    model_pending_ += frame.size();  // z kolejki do modelu: bez podwojnego liczenia miejsca
    queued_bytes_ -= frame.size();
  }
  // Bajty moga jeszcze siedziec w buforze jadra/UART: AUX=1 widziany od razu po zapisie moglby
  // oznaczac "stary" stan. Czekamy na opadniecie AUX (modul przyjal dane); kolejna ramka czeka na AUX=1.
  if (!timing_.test_ignore_aux_on_write) {
    std::ignore = WaitAux(false, timing_.aux_fall_wait, token);
  }
  return true;
}

void E220RadioDriver::TxLoop(std::stop_token token) {
  std::unique_lock<std::mutex> lock{mutex_};
  while (!token.stop_requested()) {
    const auto now = Clock::now();
    ModelAdvance(now);
    if (tx_ready_owed_ && ModelFree() >= busy_need_) {
      tx_ready_owed_ = false;
      lock.unlock();
      listener_->OnTxReady();  // poza mutexem: listener moze od razu wolac Send()
      lock.lock();
      continue;
    }
    if (!queue_.empty()) {
      std::vector<std::uint8_t> frame = std::move(queue_.front());
      queue_.pop_front();
      lock.unlock();
      const bool ok = WriteFrame(frame, token);
      lock.lock();
      cv_.notify_all();  // WaitTxQueueEmpty()
      if (!ok) {
        return;
      }
      continue;
    }
    auto wake = now + std::chrono::milliseconds(100);
    if (tx_ready_owed_) {
      // spimy do nastepnego zdarzenia modelu (koniec pakietu / start niepelnego), max 20 ms
      wake = std::max(std::min(ModelNextEvent(), now + std::chrono::milliseconds(20)),
                      now + std::chrono::milliseconds(1));
    }
    cv_.wait_until(lock, wake);
  }
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
