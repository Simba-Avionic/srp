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
#include <vector>
#include <string>
#include <bitset>
#include <thread>  // NOLINT
#include <future>  // NOLINT
#include <chrono> // NOLINT

#include "mw/i2c_service/controller/ads7828/controller.hpp"
#include "core/json/json_parser.h"
#include "core/common/error_code.h"
namespace simba {
namespace i2c {

namespace {
    // 1 - Single-Ended / 0 - Differential Inputs
    constexpr auto kInput_mode = 1;
    constexpr auto kPd_mode = 3;
    constexpr uint8_t kChannel_map[] = {0, 4, 1, 5, 2, 6, 3, 7};
    constexpr uint8_t ADS7828_ADDRESS = 0x90;
    constexpr float ADS7828_REF_VOLTAGE = 3.3f;
    constexpr float ADC_RESOLUTION = 4096.0f;  // for 12 Bit
}

ADS7828::ADS7828() {}

core::ErrorCode ADS7828::Init(std::unique_ptr<II2CController> i2c_) {
    if (!i2c_) {
        return core::ErrorCode::kInitializeError;
    }
    this->i2c_ = std::move(i2c_);
    return core::ErrorCode::kOk;
}

std::optional<uint8_t> ADS7828::GetConfigData(const uint8_t& channel) {
    if (channel > 7) {
        return {};
    }
    uint8_t res = 0;
    res |= (kInput_mode << 7);  // Select Input Mode
    res |= (kChannel_map[channel] << 4);
    res |= (kPd_mode << 2);
    return res;
}
std::optional<uint16_t> ADS7828::GetAdcRawRead(const uint8_t& channel) const {
    auto configData = GetConfigData(channel);
    if (!configData.has_value()) {
        return {};
    }
    auto res = this->i2c_->WriteRead(ADS7828_ADDRESS, configData.value(), 2);
    if (!res.has_value()) {
        return {};
    }
    uint16_t data = ((res.value()[0]) << 8);
    data |= res.value()[1];
    return data;
}
std::optional<float> ADS7828::GetAdcVoltage(const uint8_t& channel) const {
    auto res = this->GetAdcRawRead(channel);
    if (!res.has_value()) {
        return {};
    }
    return static_cast<float>((res.value()/ADC_RESOLUTION)*ADS7828_REF_VOLTAGE);
}

}  // namespace i2c
}  // namespace simba

