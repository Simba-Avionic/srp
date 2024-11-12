/**
 * @file Iuart_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CORE_UART_IUART_DRIVER_HPP_
#define CORE_UART_IUART_DRIVER_HPP_

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cstdint>
#include <vector>

#include "ara/core/result.h"

namespace simba {
namespace core {
namespace uart {

class IUartDriver {
 public:
  virtual bool Open(const std::string& portName,const int32_t& mode) = 0;
  virtual void Configure(const uint32_t& baudrate) = 0;
  virtual bool Write(const std::vector<uint8_t>& data) = 0;
  virtual bool ReadAvailable() = 0;
  virtual ara::core::Result<std::vector<uint8_t>> Read(const uint8_t& size) = 0;
  virtual void Close() = 0;
};

}  // namespace uart
}  // namespace core
}  // namespace simba

#endif  // CORE_UART_IUART_DRIVER_HPP_
