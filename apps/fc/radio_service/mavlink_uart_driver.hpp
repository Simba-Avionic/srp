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

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include "core/common/error_code.h"

namespace srp {
namespace core {
namespace uart {

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
