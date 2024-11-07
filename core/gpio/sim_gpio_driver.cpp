/**
 * @file sim_gpio_driver.cpp
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
#include "ara/log/log.h"

namespace simba {
namespace core {
namespace gpio {

GpioDriver::GpioDriver(): gpio_logger_{
    ara::log::LoggingMenager::GetInstance()->CreateLogger("GPIO", "", ara::log::LogLevel::kDebug)} {}

GpioDriver::~GpioDriver() {}

core::ErrorCode  GpioDriver::unregisterPin(uint8_t pinNumber) {
    ara::log::LogInfo() << "Unregister PIN with ID:" << pinNumber;
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::initializePin(uint8_t pinNumber, direction_t direction) {
    ara::log::LogInfo() << "Initialize PIN with ID:" << pinNumber
    << "direction:" << ((direction == direction_t::IN) ? "IN" : "OUT");
    return core::ErrorCode::kOk;
}


std::string GpioDriver::getEndpointPath(uint8_t pinNumber, std::string endpoint) {
    return this->path+"/gpio"+std::to_string(pinNumber)+"/"+endpoint;
}

core::ErrorCode GpioDriver::setValue(uint8_t pinNumber , uint8_t value) {
    ara::log::LogInfo() << "SET VALUE PIN with ID:" << pinNumber
    << "value:" << std::to_string(static_cast<uint8_t>(value));
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::setDirection(uint8_t pinNumber , direction_t direction) {
    ara::log::LogInfo() << "SET DIRECTION PIN with ID:" << pinNumber
    << "direction:" << ((direction == direction_t::IN) ? "IN" : "OUT");
    return ErrorCode::kOk;
}

uint8_t GpioDriver::getValue(uint8_t pinNumber) {
    const uint8_t value = 1;
    ara::log::LogInfo() << "GET VALUE PIN with ID:" << pinNumber
    << "VALUE:" << value;
    return value;
}

direction_t GpioDriver::getDirection(uint8_t pinNumber) {
    const direction_t direction = direction_t::IN;
    ara::log::LogInfo() << "GET VALUE PIN with ID:" << pinNumber
    << "VALUE:" << ((direction == direction_t::IN) ? "IN" : "OUT");
    return direction;
}

}  // namespace gpio
}  // namespace core
}  // namespace simba
