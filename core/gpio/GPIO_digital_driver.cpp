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
#include "core/logger/Logger.h"

namespace simba {
namespace core {
namespace gpio {

GpioDigitalDriver::GpioDigitalDriver() {}

GpioDigitalDriver::~GpioDigitalDriver() {}

core::ErrorCode  GpioDigitalDriver::unregisterPin(uint8_t pinNumber) {
    std::ofstream file;
    file.open(this->path+"/unexport");
    if (!file.is_open()) {
        return core::ErrorCode::kError;
    }
    file << std::to_string(pinNumber);
    file.close();
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDigitalDriver::initializePin(uint8_t pinNumber, direction_t direction) {
    std::ofstream file;
    file.open(this->path+"/export");
    if (!file.is_open()) {
        AppLogger::Error("Cant export pin");
        return core::ErrorCode::kError;
    }
    file << std::to_string(pinNumber);
    file.close();
    if (this->setDirection(pinNumber, direction) != core::ErrorCode::kOk) {
        AppLogger::Error("cant set direction");
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}


std::string GpioDigitalDriver::getEndpointPath(uint8_t pinNumber, std::string endpoint) {
    return this->path+"/gpio"+std::to_string(pinNumber)+"/"+endpoint;
}

core::ErrorCode GpioDigitalDriver::setValue(uint8_t pinNumber , uint8_t value) {
    std::ofstream file;
    file.open(this->getEndpointPath(pinNumber, "value"));
    if (!file.is_open()) {
        AppLogger::Error("error");
        return core::ErrorCode::kError;
    }
    file << std::to_string(value);
    file.close();
    if (this->getValue(pinNumber) != value) {
        return core::ErrorCode::kOk;
    }
    return core::ErrorCode::kError;
}

core::ErrorCode GpioDigitalDriver::setDirection(uint8_t pinNumber , direction_t direction) {
    std::ofstream file;
    file.open(this->getEndpointPath(pinNumber, "direction"));
    if (!file.is_open()) {
        return core::ErrorCode::kInitializeError;
    }
    file << (direction == direction_t::IN ? "in" : "out");
    file.close();
    return ErrorCode::kOk;
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
        return OUT;
    }
    uint8_t direction;
    file >> direction;
    file.close();
    return static_cast<direction_t>(direction);
}

}  // namespace gpio
}  // namespace core
}  // namespace simba
