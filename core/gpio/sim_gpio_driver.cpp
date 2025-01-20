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
#include "core/file/file.hpp"

namespace srp {
namespace core {
namespace gpio {
namespace {
    constexpr std::string kGpioPath = "/sys/class/gpio";
}

GpioDriver::GpioDriver(std::unique_ptr<FileHandler> file): gpio_logger_{
    ara::log::LoggingMenager::GetInstance()->CreateLogger("GPIO", "", ara::log::LogLevel::kDebug)},
    file_(std::move(file)) {
}

GpioDriver::~GpioDriver() {}

core::ErrorCode  GpioDriver::unregisterPin(const uint16_t& pinNumber) {
    ara::log::LogInfo() << "Unregister PIN with ID:" << pinNumber;
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::initializePin(const uint16_t& pinNumber, const direction_t& direction) {
    ara::log::LogInfo() << "Initialize PIN with ID:" << pinNumber
    << "direction:" << ((direction == direction_t::IN) ? "IN" : "OUT");
    return core::ErrorCode::kOk;
}


std::string GpioDriver::getEndpointPath(const uint16_t& pinNumber, const std::string& endpoint) {
    return kGpioPath + "/gpio" + std::to_string(pinNumber) + "/" + endpoint;
}

core::ErrorCode GpioDriver::setValue(const uint16_t& pinNumber , const uint8_t& value) {
    ara::log::LogInfo() << "SET VALUE PIN with ID:" << pinNumber
    << "value:" << std::to_string(static_cast<uint16_t>(value));
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::setDirection(const uint16_t& pinNumber , const direction_t& direction) {
    ara::log::LogInfo() << "SET DIRECTION PIN with ID:" << pinNumber
    << "direction:" << ((direction == direction_t::IN) ? "IN" : "OUT");
    return ErrorCode::kOk;
}

uint8_t GpioDriver::getValue(const uint16_t& pinNumber) {
    const uint8_t value = 1;
    ara::log::LogInfo() << "GET VALUE PIN with ID:" << pinNumber
    << "VALUE:" << value;
    return value;
}

direction_t GpioDriver::getDirection(const uint16_t& pinNumber) {
    const direction_t direction = direction_t::IN;
    ara::log::LogInfo() << "GET VALUE PIN with ID:" << pinNumber
    << "VALUE:" << ((direction == direction_t::IN) ? "IN" : "OUT");
    return direction;
}

}  // namespace gpio
}  // namespace core
}  // namespace srp
