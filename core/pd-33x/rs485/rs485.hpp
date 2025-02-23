/**
 * @file rs485.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef CORE_PD_33X_RS485_RS485_HPP_
#define CORE_PD_33X_RS485_RS485_HPP_
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include "core/uart/uart_driver.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "mw/gpio_server/controller/Igpio_controller.h"
#include "core/pd-33x/rs485/Irs485.hpp"
namespace srp {
namespace core {

class RS485 : public IRS485 {
 private:
    std::unique_ptr<uart::IUartDriver> uart_;
    std::unique_ptr<srp::gpio::IGPIOController> gpio_;
    uint8_t pin_id;

 public:
    bool Init(const RS485_conf_t& config,
                std::unique_ptr<uart::IUartDriver>&& uart = std::make_unique<uart::UartDriver>(),
                std::unique_ptr<srp::gpio::IGPIOController>&& gpio = std::make_unique<srp::gpio::GPIOController>()) override;
    std::optional<std::vector<uint8_t>> WriteRead(const std::vector<uint8_t>& data, const uint8_t read_size) override;
    ~RS485();
    RS485() = default;
};

}  // namespace core
}  // namespace srp

#endif  // CORE_PD_33X_RS485_RS485_HPP_
