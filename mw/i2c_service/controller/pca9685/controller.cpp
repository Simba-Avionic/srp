/**
 * @file controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <chrono>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "core/common/error_code.h"
namespace srp {
namespace i2c {

namespace {
    constexpr uint8_t PCA9685_ADDRESS = 0x70;
    constexpr uint8_t MODE1_REG = 0x00;
    constexpr uint8_t PRESCALE_REG = 0xFE;
    constexpr uint8_t PRESCALE_50_HZ_VALUE = 0x79;
    constexpr uint8_t LED_ON_L_LOOKUP[16] = {
        0x6, 0xa, 0xe, 0x12, 0x16, 0x1a, 0x1e, 0x22, 0x26, 0x2a, 0x2e, 0x32, 0x36, 0x3a, 0x3e, 0x42
    };
    constexpr uint8_t LED_ON_H_LOOKUP[16] = {
        0x7, 0xb, 0xf, 0x13, 0x17, 0x1b, 0x1f, 0x23, 0x27, 0x2b, 0x2f, 0x33, 0x37, 0x3b, 0x3f, 0x43
    };
    constexpr uint8_t LED_OFF_L_LOOKUP[16] = {
        0x8, 0xc, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x40, 0x44
    };
    constexpr uint8_t LED_OFF_H_LOOKUP[16] = {
        0x9, 0xD, 0x11, 0x15, 0x19, 0x1D, 0x21, 0x25, 0x29, 0x2D, 0x31, 0x35, 0x39, 0x3D, 0x41, 0x45
    };
}  // namespace
PCA9685::PCA9685(): pac_logger_{
    ara::log::LoggingMenager::GetInstance()->CreateLogger("pca9", "", ara::log::LogLevel::kDebug)} {
}
core::ErrorCode PCA9685::Init(std::unique_ptr<II2CController> i2c) {
  pac_logger_.LogInfo() << "PCA9685.Init: starting initialization";
  if (this->setI2C(std::move(i2c)) != core::ErrorCode::kOk) {
    pac_logger_.LogWarn() << "PCA9685.Init: failed pointer assignment";
    return core::ErrorCode::kInitializeError;
  }
  this->i2c_->Init(std::make_unique<com::soc::StreamIpcSocket>());
  if (this->InitializePCA() != core::ErrorCode::kOk) {
    pac_logger_.LogWarn() << "PCA9685.Init: failed to initialize PCA9685 chip";
    return core::ErrorCode::kInitializeError;
  }
  pac_logger_.LogInfo() << "PCA9685.Init: initialization completed";
  return core::ErrorCode::kOk;
}
core::ErrorCode PCA9685::InitializePCA() {
    if (this->i2c_->Write(PCA9685_ADDRESS, {MODE1_REG, 0x11}) != core::ErrorCode::kOk) {
        pac_logger_.LogWarn() << "PCA9685.InitializePCA: failed to write MODE1 wake command";
        return core::ErrorCode::kInitializeError;
    }
    if (this->i2c_->Write(PCA9685_ADDRESS, {PRESCALE_REG, PRESCALE_50_HZ_VALUE}) != core::ErrorCode::kOk) {
        pac_logger_.LogWarn() << "PCA9685.InitializePCA: failed to set prescaler";
        return core::ErrorCode::kInitializeError;
    }
    if (this->i2c_->Write(PCA9685_ADDRESS, {MODE1_REG, 0x81}) != core::ErrorCode::kOk) {
        pac_logger_.LogWarn() << "PCA9685.InitializePCA: failed to set MODE1 restart";
        return core::ErrorCode::kInitializeError;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    pac_logger_.LogDebug() << "PCA9685.InitializePCA: chip configured";
    return core::ErrorCode::kOk;
}


core::ErrorCode PCA9685::setI2C(std::unique_ptr<II2CController> i2c) {
  if (!i2c) {
    return core::ErrorCode::kInitializeError;
  }
  this->i2c_ = std::move(i2c);
  pac_logger_.LogDebug() << "PCA9685.setI2C: controller assigned";
  return core::ErrorCode::kOk;
}
std::vector<uint8_t> PCA9685::GenerateData(const uint8_t &channel, const uint16_t &pos) const {
    return {
    LED_ON_L_LOOKUP[channel], 0,  // ON LOW REG Val
    LED_ON_H_LOOKUP[channel], 0,   // ON HIGH REG Val
    LED_OFF_L_LOOKUP[channel], static_cast<uint8_t>(pos & 0x00FFU),  // OFF LOW REG Val
    LED_OFF_H_LOOKUP[channel], static_cast<uint8_t>(pos >> 8)};   // OFF HIGH REG Val;
}


core::ErrorCode PCA9685::SetChannelPosition(uint8_t channel, uint16_t pos) {
  pac_logger_.LogDebug() << "PCA9685.SetChannelPosition: channel " <<
                             std::to_string(static_cast<int>(channel)) <<
                             " pos " << std::to_string(pos);
  return this->i2c_->Write(PCA9685_ADDRESS, GenerateData(channel, pos));
}


uint16_t PCA9685::ComposePosition(const std::vector<uint8_t>& val) const {
    const uint8_t lsb =  val[2];
    const uint8_t msb = val[3];
    return static_cast<uint16_t>(lsb) | (static_cast<uint16_t>(msb) << 8);
}


std::optional<uint16_t> PCA9685::ReadChannelPosition(const uint8_t &channel) {
    auto val_opt = i2c_->Read(PCA9685_ADDRESS, LED_ON_L_LOOKUP[channel], 4);
    if (!val_opt.has_value()) {
        pac_logger_.LogWarn() << "PCA9685.ReadChannelPosition: failed to read channel " <<
                                  std::to_string(static_cast<int>(channel));
        return std::nullopt;
    }
    auto position = ComposePosition(val_opt.value());
    pac_logger_.LogDebug() << "PCA9685.ReadChannelPosition: channel " <<
                               std::to_string(static_cast<int>(channel)) <<
                               " pos " << std::to_string(position);
    return position;
}

}  // namespace i2c
}  // namespace srp

