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
#include <cstdint>
#include "core/uart/Iuart_driver.hpp"

namespace simba {
namespace core {
namespace uart {

class UartDriver : public IUartDriver {
 private:
  int file;
  uart_config_t config_;
 public:
  bool Open(const std::string& portName) override;
  void Configure(const uart_config_t& config) override;
  bool Write(const std::vector<uint8_t>& data) override;
  bool ReadAvailable() override;
  std::optional<std::vector<uint8_t>> Read() override;
  void Close() override;
};

}  // namespace uart
}  // namespace core
}  // namespace simba

#endif  // CORE_UART_UART_DRIVER_HPP_