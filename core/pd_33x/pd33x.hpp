/**
 * @file rs485_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef CORE_PD_33X_PD33X_HPP_
#define CORE_PD_33X_PD33X_HPP_
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include "core/uart/uart_driver.hpp"
#include "mw/gpio_server/controller/Igpio_controller.h"
#include "ara/log/log.h"
namespace srp {
namespace core {

struct d_press_t {
  float temp;
  float d_press;
};

class RS485 {
 private:
  std::unique_ptr<uart::IUartDriver> uart_;
  std::unique_ptr<gpio::IGPIOController> gpio_;

 public:
  uint16_t Generate_CRC16(const std::vector<uint8_t>& data);
  std::vector<uint8_t> add_CRC(std::vector<uint8_t> data);
  bool Init(std::unique_ptr<uart::IUartDriver> uart, std::unique_ptr<gpio::IGPIOController> gpio);
  std::optional<std::vector<uint8_t>> WriteRead(const std::vector<uint8_t>& data, const uint8_t read_size);
  float parseFloat(const std::vector<uint8_t>& data);
  std::optional<d_press_t> ReadTempAndPress();
};

}  // namespace core
}  // namespace srp

#endif  // CORE_PD_33X_PD33X_HPP_
