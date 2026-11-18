/**
 * Copyright (c) 2024
 *
 * @file fake_e220_module.hpp
 * @brief TESTONLY: podstawka modulu E220 do testow jednostkowych drivera: jeden obiekt jest
 * jednoczesnie IUartDriver i IE220Pins. Odtwarza komendy HEX trybu konfiguracji (C0/C1), AUX
 * i bufor 400 B w minimalnym zakresie potrzebnym do sprawdzenia sekwencji drivera (dokladna
 * atrapa modulu z instrukcji to e220_sim.py — testy E2E).
 */
#ifndef CORE_RADIO_E220_TESTING_FAKE_E220_MODULE_HPP_
#define CORE_RADIO_E220_TESTING_FAKE_E220_MODULE_HPP_

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>  // NOLINT
#include <cstdint>
#include <deque>
#include <mutex>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "core/radio/e220/e220_registers.hpp"
#include "core/radio/e220/i_e220_pins.hpp"
#include "core/uart/Iuart_driver.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace testing {

class FakeE220Module : public uart::IUartDriver, public IE220Pins {
 public:
  using Clock = std::chrono::steady_clock;

  struct UartOpen {
    std::string port;
    speed_t baud;
    uart::Parity parity;
  };

  // --- stan "EEPROM" i sterowanie testowe ---
  std::array<std::uint8_t, 8> eeprom{0x00, 0x00, 0x62, 0x00, 0x17, 0x03, 0x00, 0x00};  // ustawienia fabryczne (6.4)
  std::atomic<bool> aux_forced_low{false};   // AUX wiecznie niski
  std::atomic<int> aux_busy_ms{0};           // AUX niski tyle ms po kazdym zapisie danych (tryb normalny)
  std::atomic<bool> ignore_commands{false};  // modul milczy
  std::atomic<bool> format_error{false};     // odpowiada FF FF FF
  std::atomic<bool> writes_do_not_stick{false};  // C0 jest potwierdzane, ale rejestr sie nie zmienia
  std::atomic<int> fail_uart_open_count{0};  // tyle pierwszych Open() UART zwroci false
  std::atomic<bool> fail_uart_write{false};
  std::atomic<bool> uart_read_fails{false};  // Read() zwraca nullopt mimo WaitForData() == true

  // --- obserwacja ---
  std::vector<std::vector<std::uint8_t>> commands;       // wszystkie komendy HEX (C0/C1) odebrane przez modul
  std::vector<std::vector<std::uint8_t>> data_writes;    // zapisy w trybie normalnym (kazdy Write drivera)
  std::vector<UartOpen> uart_opens;
  std::vector<std::pair<bool, bool>> mode_log;           // kolejne (m0, m1)
  std::atomic<int> eeprom_writes{0};                     // liczba rejestrow zapisanych C0
  std::vector<int> written_registers;                    // adresy zapisanych C0 rejestrow
  std::atomic<int> read_commands{0};

  // --- IE220Pins ---
  bool SetMode(bool m0, bool m1) override {
    std::lock_guard<std::mutex> lock{mutex_};
    m0_ = m0;
    m1_ = m1;
    mode_log.emplace_back(m0, m1);
    return pin_set_ok;
  }
  bool ReadAux(bool* high) override {
    std::lock_guard<std::mutex> lock{mutex_};
    *high = !aux_forced_low.load() && Clock::now() >= aux_busy_until_;
    return pin_read_ok;
  }
  bool pin_set_ok{true};
  bool pin_read_ok{true};

  // --- IUartDriver ---
  bool Open(const std::string& port, const uint32_t&, const uint8_t) override {
    return Open(port, uart::UartConfig::Legacy(B9600));
  }
  bool Open(const std::string& port, const uart::UartConfig& config) override {
    std::lock_guard<std::mutex> lock{mutex_};
    if (fail_uart_open_count.load() > 0) {
      --fail_uart_open_count;
      return false;
    }
    uart_opens.push_back({port, config.baudrate, config.parity});
    open_ = true;
    baud_ = config.baudrate;
    parity_ = config.parity;
    rx_.clear();
    return true;
  }
  bool WaitForData(std::chrono::milliseconds timeout, const std::stop_token& stoken) override {
    const auto end = Clock::now() + timeout;
    for (;;) {
      {
        std::lock_guard<std::mutex> lock{mutex_};
        if (!open_) {
          return false;
        }
        if (!rx_.empty()) {
          return true;
        }
      }
      if (stoken.stop_requested() || Clock::now() >= end) {
        return false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  std::optional<std::vector<uint8_t>> Read(const uint16_t size) override {
    std::lock_guard<std::mutex> lock{mutex_};
    if (!open_ || uart_read_fails.load() || rx_.empty()) {
      return std::nullopt;
    }
    std::vector<uint8_t> out;
    const std::size_t n = std::min<std::size_t>(rx_.size(), size == 0 ? 256 : size);
    for (std::size_t i = 0; i < n; ++i) {
      out.push_back(rx_.front());
      rx_.pop_front();
    }
    return out;
  }
  void Close() override {
    std::lock_guard<std::mutex> lock{mutex_};
    open_ = false;
  }
  srp::core::ErrorCode Write(const std::vector<uint8_t>& data) override {
    std::lock_guard<std::mutex> lock{mutex_};
    if (!open_ || fail_uart_write.load()) {
      return srp::core::ErrorCode::kConnectionError;
    }
    if (m0_ && m1_) {
      ConfigModeBytes(data);
    } else {
      data_writes.push_back(data);
      tx_.insert(tx_.end(), data.begin(), data.end());
      if (aux_busy_ms.load() > 0) {
        aux_busy_until_ = Clock::now() + std::chrono::milliseconds(aux_busy_ms.load());
      }
    }
    return srp::core::ErrorCode::kOk;
  }

  // --- "radio": dane z powietrza i to, co moduł nadal ---
  void InjectRx(const std::vector<std::uint8_t>& bytes) {
    std::lock_guard<std::mutex> lock{mutex_};
    rx_.insert(rx_.end(), bytes.begin(), bytes.end());
  }
  std::vector<std::uint8_t> DrainTx(std::size_t max = 1 << 20) {
    std::lock_guard<std::mutex> lock{mutex_};
    std::vector<std::uint8_t> out;
    while (!tx_.empty() && out.size() < max) {
      out.push_back(tx_.front());
      tx_.pop_front();
    }
    return out;
  }
  Registers RegistersNow() {
    std::lock_guard<std::mutex> lock{mutex_};
    Registers r{};
    std::copy_n(eeprom.begin(), kRegisterCount, r.begin());
    return r;
  }
  bool uart_open() {
    std::lock_guard<std::mutex> lock{mutex_};
    return open_;
  }

 private:
  void ConfigModeBytes(const std::vector<std::uint8_t>& data) {
    // komendy tylko przy 9600 8N1 (6.1, str. 30)
    if (baud_ != B9600 || parity_ != uart::Parity::kNone || ignore_commands.load()) {
      return;
    }
    cmd_.insert(cmd_.end(), data.begin(), data.end());
    while (!cmd_.empty()) {
      if (cmd_[0] == 0xC1 && cmd_.size() >= 3) {
        commands.emplace_back(cmd_.begin(), cmd_.begin() + 3);
        ++read_commands;
        Reply(0xC1, cmd_[1], cmd_[2], nullptr);
        cmd_.erase(cmd_.begin(), cmd_.begin() + 3);
      } else if (cmd_[0] == 0xC0 && cmd_.size() >= 3 && cmd_.size() >= 3U + cmd_[2]) {
        const std::size_t len = cmd_[2];
        commands.emplace_back(cmd_.begin(), cmd_.begin() + 3 + len);
        std::vector<std::uint8_t> params(cmd_.begin() + 3, cmd_.begin() + 3 + len);
        const std::uint8_t start = cmd_[1];
        for (std::size_t i = 0; i < len && start + i < eeprom.size(); ++i) {
          if (!writes_do_not_stick.load()) {
            eeprom[start + i] = params[i];
          }
          ++eeprom_writes;
          written_registers.push_back(static_cast<int>(start + i));
        }
        Reply(0xC1, start, static_cast<std::uint8_t>(len), params.data());
        cmd_.erase(cmd_.begin(), cmd_.begin() + 3 + len);
      } else if (cmd_[0] != 0xC0 && cmd_[0] != 0xC1) {
        cmd_.erase(cmd_.begin());
      } else {
        break;  // niekompletna komenda
      }
    }
  }
  void Reply(std::uint8_t op, std::uint8_t start, std::uint8_t len, const std::uint8_t* params) {
    if (format_error.load()) {
      rx_.insert(rx_.end(), {0xFF, 0xFF, 0xFF});
      return;
    }
    rx_.push_back(op);
    rx_.push_back(start);
    rx_.push_back(len);
    for (std::uint8_t i = 0; i < len; ++i) {
      rx_.push_back(params != nullptr ? params[i] : (start + i < eeprom.size() ? eeprom[start + i] : 0));
    }
  }

  std::mutex mutex_;
  bool m0_{false};
  bool m1_{false};
  bool open_{false};
  speed_t baud_{B9600};
  uart::Parity parity_{uart::Parity::kNone};
  std::deque<std::uint8_t> rx_;
  std::deque<std::uint8_t> tx_;
  std::vector<std::uint8_t> cmd_;
  Clock::time_point aux_busy_until_{};
};

}  // namespace testing
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_TESTING_FAKE_E220_MODULE_HPP_
