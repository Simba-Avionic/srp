/**
 * @file Iuart_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CORE_UART_IUART_DRIVER_HPP_
#define CORE_UART_IUART_DRIVER_HPP_

#include <termios.h>
#include <chrono>  // NOLINT
#include <stop_token>  // NOLINT
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include "core/common/error_code.h"
#include "core/uart/uart_config.hpp"
namespace srp {
namespace core {
namespace uart {

class IUartDriver {
 public:
  /// Dawny interfejs (GPS, RS485/Modbus): deleguje do UartConfig::Legacy(baudrate); timeout jest ignorowany jak dotad.
  virtual bool Open(const std::string& portName, const uint32_t& baudrate = B9600, const uint8_t timeout = 10) = 0;
  /// Pelna konfiguracja portu, np. UartConfig::Mavlink(57600) dla radia.
  virtual bool Open(const std::string& portName, const UartConfig& config) = 0;
  /// Jak core::condition::wait_for: czeka do `timeout` na dane do odczytu (POLLIN). true = sa dane; false = timeout
  /// albo `stoken.stop_requested()` (wtedy wraca od razu, takze gdy stop zazadano przed wywolaniem).
  virtual bool WaitForData(std::chrono::milliseconds timeout, const std::stop_token& stoken) = 0;
  virtual std::optional<std::vector<uint8_t>> Read(const uint16_t size = 0) = 0;
  virtual void Close() = 0;
  virtual core::ErrorCode Write(const std::vector<uint8_t>& data) = 0;
  virtual ~IUartDriver() = default;
};

}  // namespace uart
}  // namespace core
}  // namespace srp

#endif  // CORE_UART_IUART_DRIVER_HPP_
