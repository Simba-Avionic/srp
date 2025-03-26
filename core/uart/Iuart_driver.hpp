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
enum BitsPerByte {
  FIVE = CS5,
  SIX = CS6,
  SEVEN = CS7,
  EIGHT = CS8
};
struct UartConfig {
  uint8_t enable_partity = 0;
  uint8_t two_stop_bits = 0;
  BitsPerByte bits = BitsPerByte::EIGHT;
  uint16_t read_wait_time = 10;  // in dS, 10 deciseconds = 1sec
};
using UartCallbackHandler =
std::function<void(const std::vector<uint8_t>)>;
class IUartDriver {
 public:
  virtual bool Open(const std::string& portName, const uint32_t& baudrate = B9600,
                                           UartConfig config = UartConfig()) = 0;
  virtual std::optional<std::vector<uint8_t>> Read(const uint16_t size = 0) = 0;
  virtual void Close() = 0;
  virtual core::ErrorCode Write(const std::vector<uint8_t>& data) = 0;
  virtual ~IUartDriver() = default;
  virtual void SetRxCallback(UartCallbackHandler rx_callback) = 0;
  virtual bool StartRxThread(const std::string& app_name) = 0;
};

}  // namespace uart
}  // namespace core
}  // namespace srp

#endif  // CORE_UART_IUART_DRIVER_HPP_
