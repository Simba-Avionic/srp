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

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <functional>
#include <thread>  // NOLINT
#include "core/uart/Iuart_driver.hpp"
#include "core/common/error_code.h"

namespace srp {
namespace core {
namespace uart {

class UartDriver: public IUartDriver {
 private:
  int serial_port;
  UartCallbackHandler rx_callback;
  std::unique_ptr<std::jthread> rx_thread;
 public:
  void SetRxCallback(UartCallbackHandler rx_callback) override;
  bool StartRxThread(const std::string& app_name) override;
  bool Open(const std::string& portName, const uint32_t& baudrate = B9600,
                                  UartConfig config = UartConfig()) override;
  std::optional<std::vector<uint8_t>> Read(const uint16_t size = 0) override;
  core::ErrorCode Write(const std::vector<uint8_t>& data) override;
  void Close() override;
};

}  // namespace uart
}  // namespace core
}  // namespace srp

#endif  // CORE_UART_UART_DRIVER_HPP_
