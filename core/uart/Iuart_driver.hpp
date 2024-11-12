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

struct uart_config_t {
  uint32_t baudrate = B9600;
  bool enablePartity = false;
  bool setTwoStopBits = false;
  uint8_t bitsPerByte = CS8;
  bool enableHardwareControl = false;
};

class IUartDriver {
 public:
  virtual bool Open(const std::string& portName) = 0;
  virtual void Configure(const uart_config_t& config) = 0;
  virtual bool Write(const std::vector<uint8_t>& data) = 0;
  virtual bool ReadAvailable() = 0;
  virtual std::optional<std::vector<uint8_t>> Read() = 0;
  virtual void Close() = 0;
};

}  // namespace uart
}  // namespace core
}  // namespace simba

#endif  // CORE_UART_IUART_DRIVER_HPP_
