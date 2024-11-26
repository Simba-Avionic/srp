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
 public:
  bool Open(const std::string& portName) noexcept override;
  void Configure(const uart_config_t& config) noexcept override;
  bool Write(const std::vector<uint8_t>& data) noexcept override;
  bool ReadAvailable() noexcept override;
  std::optional<std::vector<uint8_t>> Read() noexcept override;
  void Close() noexcept override;
};

}  // namespace uart
}  // namespace core
}  // namespace simba

#endif  // CORE_UART_UART_DRIVER_HPP_
