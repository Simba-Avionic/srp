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
    static constexpr uint8_t kOUTX_L_G = 0x22;
    static constexpr uint8_t kOUTX_L_XL = 0x28;
    static constexpr uint8_t kI2C_ADDR = 0x6A;
    static constexpr uint8_t kACCEL_SET_REG = 0x10;  // CTRL1_XL
    static constexpr uint8_t kGYRO_SET_REG = 0x11;  // CTRL2_G
    static constexpr uint8_t kSTATUS_REG = 0x1E;
    static constexpr uint8_t kWHO_AM_I_REG = 0x0F;
    static constexpr uint8_t kWHO_AM_I_EXPECTED = 0x6C;
    static constexpr uint8_t kCTRL3_C = 0x12;
    // BDU (bit 6) + IF_INC (bit 2); bit 1 must stay 0 (DS12140 Table 49).
    static constexpr uint8_t kCTRL3_C_BDU_IF_INC = 0x44;
    static constexpr uint8_t kCTRL9_XL = 0x18;
    // DEN_X/Y/Z = 1, I3C_disable = 1, reserved LSB = 0 (DS12140 Table 66–67).
    static constexpr uint8_t kCTRL9_XL_I2C_ONLY = 0xE2;

    int16_t BytesToInt16(uint8_t lo, uint8_t hi) {
        return static_cast<int16_t>(
            static_cast<uint16_t>(lo) | (static_cast<uint16_t>(hi) << 8U));
    }
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
    const auto who = i2c_->Read(kI2C_ADDR, kWHO_AM_I_REG, 1);
    if (!who.has_value() || who.value()[0] != kWHO_AM_I_EXPECTED) {
        ara::log::LogError() << "LSM6DSO WHO_AM_I read failed or unexpected value";
        return core::ErrorCode::kInitializeError;
    }
    if (i2c_->Write(kI2C_ADDR, {kCTRL9_XL, kCTRL9_XL_I2C_ONLY}) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to configure CTRL9_XL (I3C disable)";
        return core::ErrorCode::kInitializeError;
    }
    if (i2c_->Write(kI2C_ADDR, {kCTRL3_C, kCTRL3_C_BDU_IF_INC}) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to configure CTRL3_C (BDU, IF_INC)";
        return core::ErrorCode::kInitializeError;
    }
    auto err = core::ErrorCode::kOk;
    if (InitializeGyro(config) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize Gyroscope";
        err = core::ErrorCode::kInitializeError;
    }
    if (InitializeAccel(config) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize Accelerometer";
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
    const int16_t x = BytesToInt16(buffer[0], buffer[1]);
    const int16_t y = BytesToInt16(buffer[2], buffer[3]);
    const int16_t z = BytesToInt16(buffer[4], buffer[5]);
    // GetGyroSensitivity: hundredths of mdps/LSB (e.g. 875 -> 8.75 mdps/LSB, Table 2).
    const SensorData data = {
        .x = x * gyroSensitivity / 100000.0f,
        .y = y * gyroSensitivity / 100000.0f,
        .z = z * gyroSensitivity / 100000.0f
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
    const int16_t x = BytesToInt16(buffer[0], buffer[1]);
    const int16_t y = BytesToInt16(buffer[2], buffer[3]);
    const int16_t z = BytesToInt16(buffer[4], buffer[5]);
    // GetAccelSensitivity: thousandths of mg/LSB (e.g. 61 -> 0.061 mg/LSB, Table 2).
    const SensorData data = {
        .x = x * accelSensitivity / 1000000.0f,
        .y = y * accelSensitivity / 1000000.0f,
        .z = z * accelSensitivity / 1000000.0f
    };
    return data;
}

int32_t LSM6DSOX::GetAccelSensitivity(const ACCEL_FULL_SCALE scale) {
    // LA_So from Table 2 [mg/LSB] * 1000 for integer storage (0.061 -> 61).
    switch (scale) {
        case ACCEL_FULL_SCALE::k2g : return 61;
        case ACCEL_FULL_SCALE::k4g : return 122;
        case ACCEL_FULL_SCALE::k8g : return 244;
        case ACCEL_FULL_SCALE::k16g : return 488;
        default: return 61;
    }
}

int32_t LSM6DSOX::GetGyroSensitivity(const GYRO_FULL_SCALE scale) {
    // G_So from Table 2 [mdps/LSB] * 100 (8.75 -> 875). ±125 dps: 4.375 mdps/LSB.
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
