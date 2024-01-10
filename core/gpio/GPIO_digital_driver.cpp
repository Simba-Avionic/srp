/**
 * @file GPIO_digital_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <fstream>

#include "GPIO_digital_driver.h"

namespace simba {
namespace core {
namespace gpio {

GpioDigitalDriver::GpioDigitalDriver() {
}

std::string GpioDigitalDriver::getEndpointPath
(uint8_t pinNumber, std::string endpoint) {
    return this->path+std::to_string(pinNumber)+"/"+endpoint;
}

core::ErrorCode GpioDigitalDriver::setValue(uint8_t pinNumber , uint8_t value) {
    std::ofstream file;
    file.open(this->getEndpointPath(pinNumber, "value"));
    if (!file.is_open()) {
        return core::ErrorCode::kError;
    }
    if (this->getDirection(pinNumber) != OUT) {
        return core::ErrorCode::kConnectionError;
    }
    file << value;
    file.close();
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDigitalDriver::setDirection
(uint8_t pinNumber , direction_t direction) {
    std::ofstream file;
    file.open(this->getEndpointPath(pinNumber, "direction"));
    if (!file.is_open()) {
        return core::ErrorCode::kInitializeError;
    }
    if (direction == IN) {
        file << "in";
    } else {
        file << "out";
    }
    file.close();
}

core::ErrorCode GpioDigitalDriver::setActivePinLow(uint8_t pinNumber,
                                 bool value) {
    std::ofstream file;
    file.open(this->getEndpointPath(pinNumber, "active_low"));
    if (!file.is_open()) {
        return core::ErrorCode::kInitializeError;
    }
    if (this->getDirection(pinNumber) != IN) {
        return core::ErrorCode::kConnectionError;
    }
    file << value;
    file.close();
}

uint8_t GpioDigitalDriver::getValue(uint8_t pinNumber) {
    std::ifstream file;
    file.open(this->getEndpointPath(pinNumber, "value"));
    if (!file.is_open()) {
        return 0;
    }
    uint8_t value;
    file >> value;
    file.close();
    return value;
}

direction_t GpioDigitalDriver::getDirection(uint8_t pinNumber) {
    std::ifstream file;
    file.open(this->getEndpointPath(pinNumber, "direction"));
    if (!file.is_open()) {
        return;
    }
    uint8_t direction;
    file >> direction;
    file.close();
    return static_cast<direction_t>(direction);
}

bool GpioDigitalDriver::getActivePinLow(uint8_t pinNumber) {
    std::ifstream file;
    file.open(this->getEndpointPath(pinNumber, "active_low"));
    if (!file.is_open()) {
        return;
    }
    bool active;
    file >> active;
    file.close();
    return active;
}

}  // namespace gpio
}  // namespace core
}  // namespace simba
