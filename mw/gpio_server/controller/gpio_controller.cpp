/**
 * @file gpio_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "gpio_controller.hpp"

namespace simba {
namespace gpio {

void GPIOController::Init(uint16_t service_id) {
    this->service_id = service_id;
}

void GPIOController::SetPinValue(uint16_t pinID, uint8_t value) {
    static gpio::Header hdr(this->service_id, pinID, value);
    this->sock_.Transmit("SIMBA.GPIO", 0, hdr.GetBuffor());
}

uint8_t GPIOController::GetPinValue(uint16_t pinID) {
}


}  // namespace gpio
}  // namespace simba
