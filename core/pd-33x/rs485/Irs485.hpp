/**
 * @file Irs485.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef CORE_PD_33X_RS485_IRS485_HPP_
#define CORE_PD_33X_RS485_IRS485_HPP_

#include <memory>
#include <vector>
#include <optional>
 #include <string>
#include "core/uart/uart_driver.hpp"
#include "mw/gpio_server/controller/Igpio_controller.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"

namespace srp {
namespace core {

struct RS485_conf_t {
    uint8_t pin_id;
    std::string port_name;
    speed_t baudrate;
};

class IRS485 {
 public:
    virtual ~IRS485() = default;
    virtual bool Init(const RS485_conf_t& config,
        std::unique_ptr<uart::IUartDriver>&& uart = std::make_unique<uart::UartDriver>(),
        std::unique_ptr<srp::gpio::IGPIOController>&& gpio = std::make_unique<srp::gpio::GPIOController>()) = 0;
    virtual std::optional<std::vector<uint8_t>> WriteRead(const std::vector<uint8_t>& data, uint8_t read_size) = 0;
};

}  // namespace core
}  // namespace srp

#endif  // CORE_PD_33X_RS485_IRS485_HPP_
