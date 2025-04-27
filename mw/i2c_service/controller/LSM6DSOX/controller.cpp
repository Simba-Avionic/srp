/**
 * @file controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <utility>
#include "mw/i2c_service/controller/LSM6DSOX/controller.hpp"
#include "ara/log/log.h"
namespace srp {
namespace i2c {
namespace {
    static constexpr uint8_t kCTRL2_G = 0x11;
    static constexpr uint8_t kOUTX_L_G = 0x22;
    static constexpr uint8_t kOUTX_L_XL = 0x28;
    static constexpr uint8_t kI2C_ADDR = 0x6A;
    static constexpr uint8_t kACCEL_SET_REG = 0x10;  // CTRL1_XL
    static constexpr uint8_t kGYRO_SET_REG = 0x11;  // CTRL2_G
    static constexpr uint8_t kSTATUS_REG = 0x1E;  // STATUS_REG
}

core::ErrorCode LSM6DSOX::InitializeGyro(const config_t& config) {
    gyroSensitivity = GetGyroSensitivity(config.gyro_scale);
    uint8_t gyro_config = static_cast<uint8_t>(config.gyro_speed) | static_cast<uint8_t>(config.gyro_scale);
    return i2c_->Write(kI2C_ADDR, {kGYRO_SET_REG, gyro_config});
}

core::ErrorCode LSM6DSOX::InitializeAccel(const config_t& config) {
    accelSensitivity = GetAccelSensitivity(config.accel_scale);
    uint8_t accel_config = static_cast<uint8_t>(config.accel_speed) | static_cast<uint8_t>(config.accel_scale);
    return i2c_->Write(kI2C_ADDR, {kACCEL_SET_REG, accel_config});
}

core::ErrorCode LSM6DSOX::Initialize(std::unique_ptr<II2CController> i2c, const config_t& config) {
    this->i2c_ = std::move(i2c);
    if (i2c_->Init(std::make_unique<com::soc::StreamIpcSocket>()) != core::ErrorCode::kOk) {
        return core::ErrorCode::kInitializeError;
    }
    auto err = core::ErrorCode::kOk;
    if (InitializeGyro(config) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initiaize Gyroscope";
        err = core::ErrorCode::kInitializeError;
    }
    if (InitializeAccel(config) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initiaize Accelerometr";
        err = core::ErrorCode::kInitializeError;
    }
    return err;
}
std::optional<SensorData> LSM6DSOX::ReadGyroData() {
    const auto status = this->i2c_->Read(kI2C_ADDR, kSTATUS_REG, 1);
    if (!status.has_value() || (status.value()[0] & 0x02) == 0) {
        return std::nullopt;
    }
    const auto buffer_opt = this->i2c_->Read(kI2C_ADDR, kOUTX_L_G, 6);
    if (!buffer_opt.has_value()) {
        return std::nullopt;
    }
    const auto buffer = buffer_opt.value();
    const int16_t x = buffer[1] << 8 | buffer[0];
    const int16_t y = buffer[3] << 8 | buffer[2];
    const int16_t z = buffer[5] << 8 | buffer[4];
    const SensorData data = {
        .x = x * gyroSensitivity / 100.0f,
        .y = y * gyroSensitivity / 100.0f,
        .z = z * gyroSensitivity / 100.0f
    };
    return data;
}
std::optional<SensorData> LSM6DSOX::ReadAccelData() {
    const auto status = this->i2c_->Read(kI2C_ADDR, kSTATUS_REG, 1);
    if (!status.has_value() || (status.value()[0] & 0x01) == 0) {
        return std::nullopt;
    }
    const auto buffer_opt = this->i2c_->Read(kI2C_ADDR, kOUTX_L_XL, 6);
    if (!buffer_opt.has_value()) {
        return std::nullopt;
    }
    const auto buffer = buffer_opt.value();
    const int16_t x = buffer[1] << 8 | buffer[0];
    const int16_t y = buffer[3] << 8 | buffer[2];
    const int16_t z = buffer[5] << 8 | buffer[4];
    const SensorData data = {
        .x = x * accelSensitivity / 1000.0f,
        .y = y * accelSensitivity / 1000.0f,
        .z = z * accelSensitivity / 1000.0f
    };
    return data;
}

int32_t LSM6DSOX::GetAccelSensitivity(const ACCEL_FULL_SCALE scale) {
    switch (scale) {
        case ACCEL_FULL_SCALE::k2g : return 61;
        case ACCEL_FULL_SCALE::k4g : return 122;
        case ACCEL_FULL_SCALE::k8g : return 244;
        case ACCEL_FULL_SCALE::k16g : return 488;
        default: return 61;
    }
}

int32_t LSM6DSOX::GetGyroSensitivity(const GYRO_FULL_SCALE scale) {
    switch (scale) {
        case GYRO_FULL_SCALE::k250 : return 875;
        case GYRO_FULL_SCALE::k500 : return 1750;
        case GYRO_FULL_SCALE::k1000 : return 3500;
        case GYRO_FULL_SCALE::k2000 : return 7000;
        default: return 875;
    }
}
}  // namespace i2c
}  // namespace srp
