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
}
namespace {  // CONFIG
    constexpr uint8_t PD0 = 0;
    constexpr uint8_t PD1 = 0;
}

ADS7828::ADS7828() {}
void ADS7828::Init(const uint16_t &service_id, const std::unordered_map<std::string, std::string>& parms) {
    this->i2c_.Init(service_id);
}

uint8_t ADS7828::GetConfigData(const uint8_t& channel) {
    uint8_t res = 0;  // [0:1] unused
    res |= (PD0 << 2);
    res |= (PD1 << 3);
    res |= (channel << 6);
    res |= (1 << 7);  // [7] Single-Ennded / Differencial Input
    return res;
}
std::optional<uint16_t> ADS7828::GetAdcRawRead(const uint8_t& channel) {
    auto res = this->i2c_.WriteRead(ADS7828_ADDRESS, 2, GetConfigData(channel));
    if (!res.has_value()) {
        return {};
    }
    uint16_t data = ((res.value()[0] & 0x000f) << 8);
    data += (res.value()[1] & 0xff);
    return data;
}
std::optional<float> ADS7828::GetAdcVoltage(const uint8_t& channel) {
    auto res = this->GetAdcRawRead(channel);
    if (!res.has_value()) {
        return {};
    }
    return static_cast<float>((res.value()/4096)*ADS7828_REF_VOLTAGE);
}

}  // namespace i2c
}  // namespace simba

