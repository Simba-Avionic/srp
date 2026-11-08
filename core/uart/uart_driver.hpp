/**
 * @file uart_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CORE_UART_UART_DRIVER_HPP_
#define CORE_UART_UART_DRIVER_HPP_

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <optional>
#include "core/uart/Iuart_driver.hpp"
#include "core/uart/uart_config.hpp"
#include "core/common/error_code.h"
#include "ara/log/logging_menager.h"

namespace srp {
namespace core {
namespace uart {

class UartDriver: public IUartDriver {
 private:
  int serial_port;
  UartConfig config_;
  const ara::log::Logger& logger_;

 public:
  UartDriver();
  bool Open(const std::string& portName, const uint32_t& baudrate = B9600, const uint8_t timeout = 10) override;
  bool Open(const std::string& portName, const UartConfig& config) override;
  bool WaitForData(int timeout_ms) override;
  std::optional<std::vector<uint8_t>> Read(const uint16_t size = 0) override;
  core::ErrorCode Write(const std::vector<uint8_t>& data) override;
  void Close() override;
};

}  // namespace uart
}  // namespace core
}  // namespace srp

#endif  // CORE_UART_UART_DRIVER_HPP_
