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
#include <utility>
#include "gpio_driver.hpp"

namespace srp {
namespace core {
namespace gpio {

namespace {
    constexpr std::string kGpioPath = "/sys/class/gpio";
}

GpioDriver::GpioDriver(std::unique_ptr<FileHandler> file): file_(std::move(file)), gpio_logger_{
    ara::log::LoggingMenager::GetInstance()->CreateLogger("gpio", "", ara::log::LogLevel::kInfo)} {}

GpioDriver::~GpioDriver() {}

core::ErrorCode GpioDriver::unregisterPin(const uint16_t& pinNumber) {
    if (file_->open(kGpioPath + "/unexport", File_mode_t::WRITE)) {
        return core::ErrorCode::kError;
    }
    if (!file_->write(std::to_string(pinNumber))) {
        return core::ErrorCode::kError;
    }
    file_->close();
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::initializePin(const uint16_t& pinNumber, const direction_t& direction) {
    if (!file_->open(kGpioPath + "/export", File_mode_t::WRITE)) {
        gpio_logger_.LogError() <<
        gpio_logger_.LogError() <<("Cant export pin");
        return core::ErrorCode::kError;
    }
    if (!file_->write(std::to_string(pinNumber))) {
        return core::ErrorCode::kError;
    }
    file_->close();
    if (this->setDirection(pinNumber, direction) != core::ErrorCode::kOk) {
        gpio_logger_.LogError() <<("cant set direction");
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}


std::string GpioDriver::getEndpointPath(const uint16_t& pinNumber, const std::string& endpoint) {
    return kGpioPath + "/gpio" + std::to_string(pinNumber) + "/" + endpoint;
}

core::ErrorCode GpioDriver::setValue(const uint16_t& pinNumber , const uint8_t& value) {
    if (!file_->open(this->getEndpointPath(pinNumber, "value"), File_mode_t::WRITE)) {
        return core::ErrorCode::kError;
    }
    if (!file_->write(std::to_string(value))) {
        return core::ErrorCode::kError;
    }
    file_->close();
    if (this->getValue(pinNumber) != value) {
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::setDirection(const uint16_t& pinNumber , const direction_t& direction) {
    if (!file_->open(this->getEndpointPath(pinNumber, "direction"), File_mode_t::WRITE)) {
        return core::ErrorCode::kError;
    }
    if (!file_->write((direction == direction_t::IN ? "in" : "out"))) {
        return core::ErrorCode::kError;
    }
    file_->close();
    return ErrorCode::kOk;
}

uint8_t GpioDriver::getValue(const uint16_t& pinNumber) {
    if (!file_->open(this->getEndpointPath(pinNumber, "value"), File_mode_t::READ)) {
        return core::ErrorCode::kError;
    }
    auto val = file_->read();
    if (!val.has_value()) {
        return core::ErrorCode::kError;
    }
    file_->close();
    return atoi(val.value().c_str());
}

direction_t GpioDriver::getDirection(const uint16_t& pinNumber) {
    if (!file_->open(this->getEndpointPath(pinNumber, "direction"), File_mode_t::READ)) {
        return direction_t::ERROR;
    }
    auto val = file_->read();
    file_->close();
    if (!val.has_value()) {
        return direction_t::ERROR;
    }
    return static_cast<direction_t>(atoi(val.value().c_str()));
}

}  // namespace gpio
}  // namespace core
}  // namespace srp
