/**
 * @file gpio_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <fstream>
#include "gpio_driver.hpp"

namespace simba {
namespace core {
namespace gpio {

GpioDriver::GpioDriver(): gpio_logger_{
    ara::log::LoggingMenager::GetInstance()->CreateLogger("gpio", "", ara::log::LogLevel::kInfo)} {}

GpioDriver::~GpioDriver() {}

core::ErrorCode GpioDigitalDriver::unregisterPin(const uint16_t& pinNumber) {
    std::ofstream file;
    file.open(this->path+"/unexport");
    if (!file.is_open()) {
        return core::ErrorCode::kError;
    }
    file << std::to_string(pinNumber);
    file.close();
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDigitalDriver::initializePin(const uint16_t& pinNumber, const direction_t& direction) {
    std::ofstream file;
    file.open(this->path+"/export");
    if (!file.is_open()) {
        gpio_logger_.LogError() <<
        gpio_logger_.LogError() <<("Cant export pin");
        return core::ErrorCode::kError;
    }
    file << std::to_string(pinNumber);
    file.close();
    if (this->setDirection(pinNumber, direction) != core::ErrorCode::kOk) {
        gpio_logger_.LogError() <<("cant set direction");
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}


std::string GpioDigitalDriver::getEndpointPath(const uint16_t& pinNumber, const std::string& endpoint) {
    return this->path+"/gpio"+std::to_string(pinNumber)+"/"+endpoint;
}

core::ErrorCode GpioDigitalDriver::setValue(const uint16_t& pinNumber , const uint8_t& value) {
    std::ofstream file;
    file.open(this->getEndpointPath(pinNumber, "value"));
    if (!file.is_open()) {
        gpio_logger_.LogError() <<("error");
        return core::ErrorCode::kError;
    }
    file << std::to_string(value);
    file.close();
    if (this->getValue(pinNumber) != value) {
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDigitalDriver::setDirection(const uint16_t& pinNumber , const direction_t& direction) {
    std::ofstream file;
    file.open(this->getEndpointPath(pinNumber, "direction"));
    if (!file.is_open()) {
        return core::ErrorCode::kInitializeError;
    }
    file << (direction == direction_t::IN ? "in" : "out");
    file.close();
    return ErrorCode::kOk;
}

uint8_t GpioDigitalDriver::getValue(const uint16_t& pinNumber) {
    std::ifstream file;
    file.open(this->getEndpointPath(pinNumber, "value"));
    if (!file.is_open()) {
        return 2;
    }
    std::string value;
    file >> value;
    file.close();
    gpio_logger_.LogDebug() << ("Value is:" + value);
    return atoi(value.c_str());
}

direction_t GpioDigitalDriver::getDirection(const uint16_t& pinNumber) {
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
