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
#include "core/logger/Logger.h"
namespace simba {
namespace i2c {

namespace {
    constexpr uint8_t ADS7828_ADDRESS = 0x90;
    constexpr uint8_t ADS7828_REF_VOLTAGE = 5;
    constexpr auto ADC_RESOLUTION = 4096;  // for 12 Bit
}
namespace {  // CONFIG
    constexpr uint8_t PD0 = 0;
    constexpr uint8_t PD1 = 0;
}

ADS7828::ADS7828() {}

std::optional<uint8_t> ADS7828::GetConfigData(const uint8_t& channel) {
    uint8_t rawChannel;
    switch (channel) {
        case 0:
        rawChannel = 0;
        break;
        case 1:
        rawChannel = 4;
        break;
        case 2:
        rawChannel = 1;
        break;
        case 3:
        rawChannel = 5;
        break;
        case 4:
        rawChannel = 2;
        break;
        case 5:
        rawChannel = 6;
        break;
        case 6:
        rawChannel = 3;
        break;
        case 7:
        rawChannel = 7;
        break;
        default:
        return {};
    }

    uint8_t res = 0;  // [0:1] unused
    res |= (PD0 << 2);
    res |= (PD1 << 3);
    res |= (rawChannel << 6);
    res |= (1 << 7);  // [7] Single-Ennded / Differencial Input
    return res;
}
std::optional<uint16_t> ADS7828::GetAdcRawRead(const uint8_t& channel) {
    auto configData = GetConfigData(channel);
    if (!configData.has_value()) {
        return {};
    }
    auto res = this->i2c_.WriteRead(ADS7828_ADDRESS, 2, configData.value());
    if (!res.has_value()) {
        return {};
    }
    uint16_t data = ((res.value()[0]) << 8);
    data |= res.value()[1];
    return data;
}
std::optional<float> ADS7828::GetAdcVoltage(const uint8_t& channel) {
    auto res = this->GetAdcRawRead(channel);
    if (!res.has_value()) {
        return {};
    }
    return static_cast<float>((res.value()/ADC_RESOLUTION)*ADS7828_REF_VOLTAGE);
}

}  // namespace i2c
}  // namespace simba

