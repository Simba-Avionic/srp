/**
 * @file controller.cpp
 * @author Grzegorz Horbaczewski (gregority9@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <math.h>
#include <chrono>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <utility>
#include <vector>
#include <algorithm>

#include "core/common/condition.h"
#include "mw/i2c_service/controller/bme280/controller.hpp"
#include "core/common/error_code.h"
namespace srp {
namespace i2c {

namespace {
    static constexpr uint8_t BME280_ADDRESS = 0x76;
    static constexpr uint8_t CTRL_HUM = 0xF2;
    static constexpr uint8_t CTRL_MEAS = 0xF4;
    static constexpr uint8_t CONFIG = 0xF5;
    static constexpr uint8_t PRESSURE_OUTPUT = 0xF7;
    static constexpr uint8_t TRIMMING_REGISTER_1 = 0x88;
    static constexpr uint8_t TRIMMING_REGISTER_2 = 0xA1;
    static constexpr uint8_t TRIMMING_REGISTER_3 = 0xE1;
    static constexpr uint8_t CTRL_MEAS_DEFAULT_VALUE = 0x37;
    static constexpr uint8_t CONFIG_DEFAULT_VALUE = 0x20;

    static constexpr uint8_t SIZE_OF_TRIMMING_REGISTER_1 = 24;
    static constexpr uint8_t SIZE_OF_TRIMMING_REGISTER_2 = 1;
    static constexpr uint8_t SIZE_OF_TRIMMING_REGISTER_3 = 8;
    static constexpr int32_t HUMIDITY_MAX = 419430400;

    static constexpr float BME_PRESSURE_SCALING = 25600.0f;
    static constexpr float BME_TEMP_SCALING = 100.0f;
    static constexpr float BME_HUM_SCALING = 1024.0f;
}  // namespace

BME280::BME280(): bme_logger_{
    ara::log::LoggingMenager::GetInstance()->CreateLogger("BME-", "", ara::log::LogLevel::kInfo)} {
}

core::ErrorCode BME280::Init(std::unique_ptr<II2CController> i2c) {
  bme_logger_.LogInfo() << "BME280.Init: starting initialization";
  if (this->setI2C(std::move(i2c)) != core::ErrorCode::kOk) {
    bme_logger_.LogWarn() << "BME280.Init: failed pointer assignment";
    return core::ErrorCode::kInitializeError;
  }
  this->i2c_->Init(std::make_unique<com::soc::StreamIpcSocket>());
  if (this->InitializeBME() != core::ErrorCode::kOk) {
    bme_logger_.LogWarn() << "BME280.Init: failed to initialize BME280 chip";
    return core::ErrorCode::kInitializeError;
  }
  if (this->readTrimmingParameters() != core::ErrorCode::kOk) {
    bme_logger_.LogWarn() << "BME280.Init: failed to read trimming parameters for BME280 chip";
    return core::ErrorCode::kInitializeError;
  }
  bme_logger_.LogInfo() << "BME280.Init: initialization completed";
  return core::ErrorCode::kOk;
}
core::ErrorCode BME280::InitializeBME() {
    bme_logger_.LogDebug() << "BME280.InitializeBME: Starting initialization of BME registers";
    // Setting oversampling to: pressure x16, temperature x1, humidity x1 -> 38ms time of measurement
    // Setting work mode to normal and t_standby to 62.5ms
    // Setting IIR FILTER to OFF -> i have no idea what this does ://
    auto hum_oversampling = this->i2c_->Read(BME280_ADDRESS, CTRL_HUM, 1);
    if (!hum_oversampling.has_value() || hum_oversampling.value().size() != 1) {
        bme_logger_.LogInfo() << "BME280.InitalizeBME: Not able to read hum_oversampling setting";
        return core::ErrorCode::kError;
    }
    hum_oversampling.value()[0] |= (1<<0);
    hum_oversampling.value()[0] &= ~(3<<1);
    if (this->i2c_->Write(BME280_ADDRESS, {CTRL_HUM, hum_oversampling.value()[0]}) != core::ErrorCode::kOk) {
        bme_logger_.LogWarn() << "BME280.InitalizeBME: failed to set humidity oversampling";
        return core::ErrorCode::kInitializeError;
    }
    if (this->i2c_->Write(BME280_ADDRESS, {CTRL_MEAS, CTRL_MEAS_DEFAULT_VALUE}) != core::ErrorCode::kOk) {
        bme_logger_.LogWarn() << "BME280.InitalizeBME: failed to set ctrl meas register";
        return core::ErrorCode::kInitializeError;
    }
    if (this->i2c_->Write(BME280_ADDRESS, {CONFIG, CONFIG_DEFAULT_VALUE}) != core::ErrorCode::kOk) {
        bme_logger_.LogWarn() << "BME280.InitalizeBME: failed to set ctrl meas register";
        return core::ErrorCode::kInitializeError;
    }
    core::condition::wait_for(std::chrono::milliseconds(10), std::stop_token{});
    bme_logger_.LogDebug() << "BME280.InitalizeBME: chip configured successfully";
    return core::ErrorCode::kOk;
}


core::ErrorCode BME280::setI2C(std::unique_ptr<II2CController> i2c) {
  if (!i2c) {
    return core::ErrorCode::kInitializeError;
  }
  this->i2c_ = std::move(i2c);
  bme_logger_.LogDebug() << "BME280.setI2C: controller assigned";
  return core::ErrorCode::kOk;
}

std::optional<float> BME280::getTemperature() {
    auto outputData = readOutputData();
    auto tempRawData = extractMeasurementField(outputData, 20, 20);
    if (!tempRawData.has_value()) {
        bme_logger_.LogDebug() << "BME280.getTemperature: Error getting data for conversion";
        return std::nullopt;
    }

    int32_t var1, var2;
    var1 = ((((tempRawData.value() >> 3) -
            (static_cast<int32_t>(param.dig_T1 << 1))) *
            (static_cast<int32_t>(param.dig_T2))) >> 11);

    var2 = (((((tempRawData.value() >> 4) -
            (static_cast<int32_t>(param.dig_T1))) *
            ((tempRawData.value() >> 4) -
            (static_cast<int32_t>(param.dig_T1)))) >> 12) *
            (static_cast<int32_t>(param.dig_T3))) >> 14;

    this->t_fine = var1 + var2;

    int32_t temp = ((this->t_fine * 5 + 128) >> 8);
    return static_cast<float>(temp) / BME_TEMP_SCALING;
}

std::optional<float> BME280::getPressure() {
    auto outputData = readOutputData();

    auto tempRawData = extractMeasurementField(outputData, 20, 20);
    auto pressRawData = extractMeasurementField(outputData, 44, 20);

    if (!tempRawData.has_value() || !pressRawData.has_value()) {
        bme_logger_.LogDebug() << "BME280.getPressure: Error getting data for conversion";
        return std::nullopt;
    }

    int32_t tempVar1, tempVar2;
    tempVar1 = ((((tempRawData.value() >> 3) -
                (static_cast<int32_t>(param.dig_T1 << 1))) *
                (static_cast<int32_t>(param.dig_T2))) >> 11);

    tempVar2 = (((((tempRawData.value() >> 4) -
                (static_cast<int32_t>(param.dig_T1))) *
                ((tempRawData.value() >> 4) -
                (static_cast<int32_t>(param.dig_T1)))) >> 12) *
                (static_cast<int32_t>(param.dig_T3))) >> 14;

    this->t_fine = tempVar1 + tempVar2;

    int64_t var1, var2, p;
    var1 = (static_cast<int64_t>(this->t_fine)) - 128000;
    var2 = var1 * var1 * static_cast<int64_t>(param.dig_P6);
    var2 = var2 + ((var1 * static_cast<int64_t>(param.dig_P5)) << 17);
    var2 = var2 + ((static_cast<int64_t>(param.dig_P4)) << 35);

    var1 = ((var1 * var1 * static_cast<int64_t>(param.dig_P3)) >> 8) +
           ((var1 * static_cast<int64_t>(param.dig_P2)) << 12);

    var1 = ((((static_cast<int64_t>(1)) << 47) + var1) *
            (static_cast<int64_t>(param.dig_P1))) >> 33;

    if (var1 == 0) {
        return std::nullopt;
    }

    p = 1048576 - pressRawData.value();
    p = (((p << 31) - var2) * 3125) / var1;

    var1 = ((static_cast<int64_t>(param.dig_P9)) * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((static_cast<int64_t>(param.dig_P8)) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + ((static_cast<int64_t>(param.dig_P7)) << 4);

    return static_cast<float>(static_cast<uint32_t>(p)) / BME_PRESSURE_SCALING;
}

std::optional<float> BME280::getHumidity() {
    
    auto outputData = readOutputData();

    auto humidityRawData = extractMeasurementField(outputData, 0, 16);
    auto tempRawData = extractMeasurementField(outputData, 20, 20);

    if (!humidityRawData.has_value() || !tempRawData.has_value()) {
        bme_logger_.LogDebug() << "BME280.getHumidity: Error getting data for conversion";
        return std::nullopt;
    }

    int32_t tempVar1, tempVar2;
    tempVar1 = ((((tempRawData.value() >> 3) -
                (static_cast<int32_t>(param.dig_T1 << 1))) *
                (static_cast<int32_t>(param.dig_T2))) >> 11);

    tempVar2 = (((((tempRawData.value() >> 4) -
                (static_cast<int32_t>(param.dig_T1))) *
                ((tempRawData.value() >> 4) -
                (static_cast<int32_t>(param.dig_T1)))) >> 12) *
                (static_cast<int32_t>(param.dig_T3))) >> 14;

    this->t_fine = tempVar1 + tempVar2;

    int32_t t_fine_adj = this->t_fine - 76800;

    int32_t part1 = (static_cast<int32_t>(humidityRawData.value()) << 14) -
                    (static_cast<int32_t>(param.dig_H4) << 20) -
                    (static_cast<int32_t>(param.dig_H5) * t_fine_adj);
    part1 = (part1 + 16384) >> 15;

    int32_t part2 = (t_fine_adj * static_cast<int32_t>(param.dig_H6)) >> 10;
    part2 = (part2 * (((t_fine_adj * static_cast<int32_t>(param.dig_H3)) >> 11) + 32768)) >> 10;
    part2 = ((part2 + 2097152) * static_cast<int32_t>(param.dig_H2) + 8192) >> 14;

    int32_t v_x1_u32r = part1 * part2;
    v_x1_u32r = v_x1_u32r -
                (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                static_cast<int32_t>(param.dig_H1)) >> 4);

    v_x1_u32r = std::max(0, v_x1_u32r);
    v_x1_u32r = std::min(v_x1_u32r, HUMIDITY_MAX);

    uint32_t hum = static_cast<uint32_t>(v_x1_u32r >> 12);
    return static_cast<float>(hum) / BME_HUM_SCALING;
}


std::optional<int32_t> BME280::extractMeasurementField(const std::optional<std::vector<uint8_t>>& input,
                                             const uint8_t startBit, const uint8_t length) {
    if (!input.has_value() || input->size() != 8) {
        return std::nullopt;
    }

    if (length == 0 || length > 31 || (startBit + length) > 64) {
        return std::nullopt;
    }

    const auto& bytes = input.value();
    uint64_t fullValue = 0;

    for (const uint8_t b : bytes) {
        fullValue = (fullValue << 8) | b;
    }
    uint64_t mask = (1ULL << length) - 1;
    uint64_t extractedValue = (fullValue >> startBit) & mask;
    return static_cast<int32_t>(extractedValue);
}

std::optional<std::vector<uint8_t>> BME280::readOutputData() {
    return this->i2c_->Read(BME280_ADDRESS, PRESSURE_OUTPUT, 8);
}


core::ErrorCode BME280::readTrimmingParameters() {
    auto temp = this->i2c_->Read(BME280_ADDRESS, TRIMMING_REGISTER_1, SIZE_OF_TRIMMING_REGISTER_1);
    if (!temp.has_value() || temp->size() != SIZE_OF_TRIMMING_REGISTER_1) {
        bme_logger_.LogError() << "BME280.readTrimmingParameters: Failed reading from trimming registers 1";
        return core::ErrorCode::kError;
    }

    param.dig_T1 = (static_cast<uint16_t>(temp.value()[1]) << 8) | temp.value()[0];
    param.dig_T2 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[3]) << 8) | temp.value()[2]);
    param.dig_T3 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[5]) << 8) | temp.value()[4]);
    param.dig_P1 = (static_cast<uint16_t>(temp.value()[7]) << 8) | temp.value()[6];
    param.dig_P2 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[9]) << 8) | temp.value()[8]);
    param.dig_P3 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[11]) << 8) | temp.value()[10]);
    param.dig_P4 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[13]) << 8) | temp.value()[12]);
    param.dig_P5 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[15]) << 8) | temp.value()[14]);
    param.dig_P6 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[17]) << 8) | temp.value()[16]);
    param.dig_P7 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[19]) << 8) | temp.value()[18]);
    param.dig_P8 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[21]) << 8) | temp.value()[20]);
    param.dig_P9 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[23]) << 8) | temp.value()[22]);

    temp = this->i2c_->Read(BME280_ADDRESS, TRIMMING_REGISTER_2, SIZE_OF_TRIMMING_REGISTER_2);
    if (!temp.has_value() || temp->size() != SIZE_OF_TRIMMING_REGISTER_2) {
        bme_logger_.LogError() << "BME280.readTrimmingParameters: Failed reading from trimming registers 2";
        return core::ErrorCode::kError;
    }
    param.dig_H1 = temp.value()[0];

    temp = this->i2c_->Read(BME280_ADDRESS, TRIMMING_REGISTER_3, SIZE_OF_TRIMMING_REGISTER_3);
    if (!temp.has_value() || temp->size() != SIZE_OF_TRIMMING_REGISTER_3) {
        bme_logger_.LogError() << "BME280.readTrimmingParameters: Failed reading from trimming registers 3";
        return core::ErrorCode::kError;
    }
    param.dig_H2 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[1]) << 8) | temp.value()[0]);
    param.dig_H3 = temp.value()[2];
    param.dig_H4 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[3]) << 4) | (temp.value()[4] & 0x0F));
    param.dig_H5 = static_cast<int16_t>((static_cast<uint16_t>(temp.value()[6]) << 4) | (temp.value()[5] >> 4));
    param.dig_H6 = temp.value()[7];

    return core::ErrorCode::kOk;
}

}  // namespace i2c
}  // namespace srp
