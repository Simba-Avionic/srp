/**
 * Copyright (c) 2024
 *
 * @file serial_port_uart.hpp
 * @brief IUartDriver on top of the platform `serial::SerialPort` (termios raw 8N1, poll(), exclusive
 * open). Unlike the srp `UartDriver` it needs no ara::log singleton, so it works inside the plugin
 * `.so` (which has its own copy of the static libraries, hence an uninitialised host logger).
 *
 * Limits of SerialPort: 8N1 only (Open() with another parity returns false) and the port is opened
 * exclusively (TIOCEXCL + flock), so a second user of the same device fails to open it.
 */
#ifndef CORE_RADIO_E220_SERIAL_PORT_UART_HPP_
#define CORE_RADIO_E220_SERIAL_PORT_UART_HPP_

#include <chrono>  // NOLINT
#include <cstdint>
#include <optional>
#include <stop_token>  // NOLINT
#include <string>
#include <vector>

#include "core/uart/Iuart_driver.hpp"
#include "core/serial/serial_port.h"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

class SerialPortUart final : public uart::IUartDriver {
 public:
  /// `baudrate` is a termios constant (B9600, ...) like in the IUartDriver contract.
  bool Open(const std::string& portName, const uint32_t& baudrate = B9600, const uint8_t timeout = 10) override;
  bool Open(const std::string& portName, const uart::UartConfig& config) override;
  bool WaitForData(std::chrono::milliseconds timeout, const std::stop_token& stoken) override;
  std::optional<std::vector<uint8_t>> Read(const uint16_t size = 0) override;
  void Close() override { port_.Close(); }
  core::ErrorCode Write(const std::vector<uint8_t>& data) override;

 private:
  serial::SerialPort port_;
};

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_SERIAL_PORT_UART_HPP_
