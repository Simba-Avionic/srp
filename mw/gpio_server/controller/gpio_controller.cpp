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


GPIOController::GPIOController(com::soc::ISocket* socket) {
    this->sock_ = socket;
}
GPIOController::GPIOController() {
    this->sock_ = new com::soc::IpcSocket();
}

core::ErrorCode GPIOController::Init(uint16_t service_id) {
    this->service_id = service_id;
    return core::ErrorCode::kOk;
}

core::ErrorCode GPIOController::SetPinValue(uint16_t pinID, Value value) {
    if (value == Value::ERROR) {
        return core::ErrorCode::kError;
    }
    if (this->sock_== nullptr) {
        return core::ErrorCode::kInitializeError;
    }
    gpio::Header hdr(this->service_id, pinID, value);
    return this->sock_->Transmit("SIMBA.GPIO.SET", 0, hdr.GetBuffor());
}

Value GPIOController::GetPinValue(uint16_t pinID) {
    return Value::HIGH;
}


}  // namespace gpio
}  // namespace simba
