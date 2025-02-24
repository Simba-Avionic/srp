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
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include "core/common/error_code.h"
namespace srp {
namespace core {
namespace uart {

class IUartDriver {
 public:
  virtual bool Open(const std::string& portName, const uint32_t& baudrate = B9600) = 0;
  virtual std::optional<std::vector<uint8_t>> Read(const uint16_t size = 0) = 0;
  virtual void Close() = 0;
  virtual core::ErrorCode Write(const std::vector<uint8_t>& data) = 0;
  virtual ~IUartDriver() = default;
};

}  // namespace uart
}  // namespace core
}  // namespace srp

#endif  // CORE_UART_IUART_DRIVER_HPP_
