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

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "gpio_controller.hpp"

namespace simba {
namespace gpio {

void GPIOController::Init(uint16_t service_id) {
    this->service_id = service_id;
}

core::ErrorCode GPIOController::SetPinValue(uint16_t pinID, Value value) {
    gpio::Header hdr(this->service_id, pinID, value);
    return this->sock_.Transmit("SIMBA.GPIO.SET", 0, hdr.GetBuffor());
}

Value GPIOController::GetPinValue(uint16_t pinID) {

    return Value::HIGH;
}


}  // namespace gpio
}  // namespace simba
