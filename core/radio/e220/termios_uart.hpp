/**
 * Copyright (c) 2024
 *
 * @file termios_uart.hpp
 * @brief IUartDriver na termios + poll() BEZ ara::log (plugin `.so` ma wlasna kopie statycznych
 * bibliotek, wiec singleton loggera hosta jest dla niego niezainicjowany — srp `UartDriver`
 * wola go w konstruktorze i w pluginie by padl).
 *
 * Raw 8 bitow, parzystosc z `UartConfig`, 1 bit stopu, bez flow control. Nie ustawia TIOCEXCL
 * (ponowne otwarcie po Stop()/Start() daemona nie moze trafic na wlasny zostawiony tryb wylaczny).
 */
#ifndef CORE_RADIO_E220_TERMIOS_UART_HPP_
#define CORE_RADIO_E220_TERMIOS_UART_HPP_

#include <atomic>
#include <chrono>  // NOLINT
#include <cstdint>
#include <mutex>  // NOLINT
#include <optional>
#include <stop_token>  // NOLINT
#include <string>
#include <vector>

#include "core/uart/Iuart_driver.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

class TermiosUart final : public uart::IUartDriver {
 public:
  TermiosUart() = default;
  ~TermiosUart() override { Close(); }

  bool Open(const std::string& portName, const uint32_t& baudrate = B9600, const uint8_t timeout = 10) override;
  bool Open(const std::string& portName, const uart::UartConfig& config) override;
  bool WaitForData(std::chrono::milliseconds timeout, const std::stop_token& stoken) override;
  std::optional<std::vector<uint8_t>> Read(const uint16_t size = 0) override;
  void Close() override;
  core::ErrorCode Write(const std::vector<uint8_t>& data) override;

 private:
  std::mutex open_mutex_;  // Open/Close
  std::atomic<int> fd_{-1};
};

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_TERMIOS_UART_HPP_
