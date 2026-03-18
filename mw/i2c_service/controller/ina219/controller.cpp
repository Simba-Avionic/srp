/**
 * @file controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "mw/i2c_service/controller/ina219/controller.hpp"
#include <cmath>

namespace srp {
namespace i2c {

namespace {
    static constexpr uint8_t kConfiguration_register_address = 0x00;
    static constexpr uint8_t kShuntVoltage_register_address = 0x01;
    static constexpr uint8_t kBusVoltage_register_address = 0x02;
    static constexpr uint8_t kPowerMeasurement_register_address = 0x03;
    static constexpr uint8_t kCurrentMeasurement_register_address = 0x04;
    static constexpr uint8_t kCalibration_register_address = 0x05;

    static constexpr auto kBusVoltageRange = 0b0;  // 16V FSR
    static constexpr auto kPGAShuntGain = 0b01;    // GAIN /2 (+- 80mV)
    static constexpr auto kBusAdcResolutionAveranging = 0b1000;   // 12b, 2 samples
    static constexpr auto kShuntAdcResolutionAveranging = 0b1000;  // 12b, 2 samples
    static constexpr auto kMode = 0b111;           // Shunt and bus, continuous

    consteval uint16_t GetConfigurationRegister() {
        uint16_t val = 0;
        val |= (kBusVoltageRange << 13);
        val |= (kPGAShuntGain << 11);
        val |= (kBusAdcResolutionAveranging << 7);
        val |= (kShuntAdcResolutionAveranging << 3);
        val |= (kMode << 0);
        return val;
    }

    static std::vector<uint8_t> PrepareI2CData(uint8_t reg_addr, uint16_t value) {
        return { reg_addr, static_cast<uint8_t>(value >> 8), static_cast<uint8_t>(value & 0xFF) };
    }

    static constexpr double kMaxExpectedCurrent = 8.0;  // A
    static constexpr double kRShuntValue = 0.01;       // Ohm

    consteval double GetCurrentLSB() {
        return kMaxExpectedCurrent / 32768.0;
    }

    consteval uint16_t GetCalibrationRegisterValue() {
        double reg = 0.04096 / (GetCurrentLSB() * kRShuntValue);
        return static_cast<uint16_t>(reg);
    }
}  // namespace

INA219::INA219() : ina219_logger{
    ara::log::LoggingMenager::GetInstance()->CreateLogger("INA-", "", ara::log::LogLevel::kInfo)} {}

core::ErrorCode INA219::InitDriver() {
    if (i2c_.Init(std::make_unique<com::soc::StreamIpcSocket>()) != core::ErrorCode::kOk) {
        ina219_logger.LogError() << "Failed to initialize i2c service";
        return core::ErrorCode::kInitializeError;
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode INA219::Initialize(const uint8_t ina219_address) {
    auto config_data = PrepareI2CData(kConfiguration_register_address, GetConfigurationRegister());
    if (i2c_.Write(ina219_address, config_data) != core::ErrorCode::kOk) {
        ina219_logger.LogError() << "Failed to write Configuration register";
        return core::ErrorCode::kConnectionError;
    }

    auto cal_data = PrepareI2CData(kCalibration_register_address, GetCalibrationRegisterValue());
    if (i2c_.Write(ina219_address, cal_data) != core::ErrorCode::kOk) {
        ina219_logger.LogError() << "Failed to write Calibration register";
        return core::ErrorCode::kConnectionError;
    }

    return core::ErrorCode::kOk;
}

std::optional<float> INA219::GetCurrent(const uint8_t ina219_address) {
    auto res = i2c_.Read(ina219_address, kCurrentMeasurement_register_address, 2);
    if (!res || res->size() < 2) return std::nullopt;

    int16_t raw_current = (static_cast<int16_t>((*res)[0]) << 8) | (*res)[1];

    return static_cast<float>(raw_current * GetCurrentLSB());
}

std::optional<float> INA219::GetVoltage(const uint8_t ina219_address) {
    auto res = i2c_.Read(ina219_address, kBusVoltage_register_address, 2);
    if (!res || res->size() < 2) return std::nullopt;

    uint16_t raw_reg = (static_cast<uint16_t>((*res)[0]) << 8) | (*res)[1];

    uint16_t shifted_value = raw_reg >> 3;

    return static_cast<float>(shifted_value * 0.004f);
}

}  // namespace i2c
}  // namespace srp
