/**
 * @file uart_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_UART_UART_DRIVER_HPP_
#define CORE_UART_UART_DRIVER_HPP_

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include "core/uart/Iuart_driver.hpp"

namespace srp {
namespace core {
namespace uart {

class UartDriver: public IUartDriver {
 private:
  int serial_port;

 public:
  bool Open(const std::string& portName, const speed_t& baudrate = B9600) override;
  std::optional<std::vector<char>> Read(const uint16_t size = 0) override;
  void Close() override;
};

}  // namespace uart
}  // namespace core
}  // namespace srp

#endif  // CORE_UART_UART_DRIVER_HPP_
