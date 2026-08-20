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
#include <mutex>  // NOLINT
#include <utility>
#include "gpio_driver.hpp"
#include "ara/log/log.h"
namespace srp {
namespace core {
namespace gpio {

namespace {
    constexpr std::string kGpioPath = "/sys/class/gpio";
}

GpioDriver::GpioDriver(std::unique_ptr<IFileHandler> file): file_(std::move(file))
, gpio_logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger("gpio", "", ara::log::LogLevel::kInfo)}
{}

GpioDriver::~GpioDriver() {}

std::unique_lock<std::mutex> GpioDriver::maybeLock(bool use_lock) {
    if (use_lock) {
        return std::unique_lock<std::mutex>(file_mutex_);
    }
    return std::unique_lock<std::mutex>(file_mutex_, std::defer_lock);
}

core::ErrorCode GpioDriver::unregisterPin(const uint16_t& pinNumber, bool use_lock) {
    auto lock = maybeLock(use_lock);
    if (!file_->open(kGpioPath + "/unexport", FileMode::WRITE)) {
        return core::ErrorCode::kConnectionError;
    }
    if (!file_->write(std::to_string(pinNumber))) {
        return core::ErrorCode::kError;
    }
    file_->close();
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::initializePin(const uint16_t& pinNumber, const direction_t& direction,
                                          bool use_lock) {
    auto lock = maybeLock(use_lock);
    if (!file_->open(kGpioPath + "/export", FileMode::WRITE)) {
        // gpio_logger_.LogError() <<("Cant export pin");
        return core::ErrorCode::kError;
    }
    if (!file_->write(std::to_string(pinNumber))) {
        return core::ErrorCode::kError;
    }
    file_->close();
    if (this->setDirection(pinNumber, direction, false) != core::ErrorCode::kOk) {
        // gpio_logger_.LogError() <<("cant set direction");
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}


std::string GpioDriver::getEndpointPath(const uint16_t& pinNumber, const std::string& endpoint) {
    return kGpioPath + "/gpio" + std::to_string(pinNumber) + "/" + endpoint;
}

core::ErrorCode GpioDriver::setValue(const uint16_t& pinNumber , const uint8_t& value,
                                     bool use_lock) {
    auto lock = maybeLock(use_lock);
    if (!file_->open(this->getEndpointPath(pinNumber, "value"), FileMode::WRITE)) {
        return core::ErrorCode::kError;
    }
    bool result = file_->write(std::to_string(value), false);
    file_->close();
    return result ? core::ErrorCode::kOk : core::ErrorCode::kError;
}

core::ErrorCode GpioDriver::setDirection(const uint16_t& pinNumber , const direction_t& direction,
                                         bool use_lock) {
    auto lock = maybeLock(use_lock);
    if (!file_->open(this->getEndpointPath(pinNumber, "direction"), FileMode::WRITE)) {
        return core::ErrorCode::kError;
    }
    if (!file_->write((direction == direction_t::IN ? "in" : "out"))) {
        return core::ErrorCode::kError;
    }
    file_->close();
    return ErrorCode::kOk;
}

uint8_t GpioDriver::getValue(const uint16_t& pinNumber, bool use_lock) {
    auto lock = maybeLock(use_lock);
    if (!file_->open(this->getEndpointPath(pinNumber, "value"), FileMode::READ)) {
        return core::ErrorCode::kError;
    }
    auto val = file_->read();
    if (!val.has_value()) {
        return core::ErrorCode::kError;
    }
    file_->close();
    return atoi(val.value().c_str());
}

direction_t GpioDriver::getDirection(const uint16_t& pinNumber, bool use_lock) {
    auto lock = maybeLock(use_lock);
    if (!file_->open(this->getEndpointPath(pinNumber, "direction"), FileMode::READ)) {
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
