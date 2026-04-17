/**
 * @file mavlink_uart_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_FC_RADIO_SERVICE_MAVLINK_UART_DRIVER_HPP_
#define APPS_FC_RADIO_SERVICE_MAVLINK_UART_DRIVER_HPP_

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include "core/common/error_code.h"

namespace srp {
namespace core {
namespace uart {
constexpr speed_t GetBaudrateConstant(uint32_t baudrate) {
  switch (baudrate) {
      case 57600:   return B57600;
      case 115200:  return B115200;
      case 230400:  return B230400;
      case 460800:  return B460800;
      case 500000:  return B500000;
      case 576000:  return B576000;
      case 921600:  return B921600;
      case 1000000: return B1000000;
      case 1152000: return B1152000;
      case 1500000: return B1500000;
      case 2000000: return B2000000;
      case 2500000: return B2500000;
      case 3000000: return B3000000;
      case 3500000: return B3500000;
      case 4000000: return B4000000;
      default:      return 0;
  }
}

class MavUartDriver{
 private:
  int serial_port;

 public:
  bool WaitForData(int timeout_ms);
  bool Open(const std::string& portName, const uint32_t& baudrate, const uint8_t timeout = 10);
  std::optional<std::vector<uint8_t>> Read(const uint16_t size = 0);
  core::ErrorCode Write(const std::vector<uint8_t>& data);
  void Close();
};

}  // namespace uart
}  // namespace core
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_MAVLINK_UART_DRIVER_HPP_
