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

#include "mw/i2c_service/controller/max11615/controller.hpp"
#include "core/json/json_parser.h"
#include "core/common/error_code.h"
#include "core/logger/Logger.h"
namespace simba {
namespace i2c {

namespace {
    constexpr uint8_t MAX11615_ADDRESS = 0x70;
}
namespace {
    constexpr uint8_t SCAN0 = 0;
    constexpr uint8_t SCAN1 = 0;
    constexpr uint8_t UNIPOLAR_MODE = 0;

}

MAX11615::MAX11615() {}
void MAX11615::Init(const uint16_t &service_id, const std::unordered_map<std::string, std::string>& parms) {
    this->i2c_.Init(service_id);
    this->i2c_.Write(MAX11615_ADDRESS, this->GetSetupData());
}
uint8_t MAX11615::GetSetupData(bool reset) {
    uint8_t res = 0;  // [0] dont care
    res |= (!reset << 1);  // [1] reset config bit
    res |= (UNIPOLAR_MODE << 2);  // [2]  ! for BIPolar
    res |= (1 << 7);
    return res;
}
uint8_t MAX11615::GetConfigData(uint8_t channel) {
    uint8_t res = 1;  // [0] set to single-ended=mode
    res |= (channel << 4);  // [1:4] Channel Select
    res |= (SCAN0 << 5);  // [5] Scan select Bit 0
    res |= (SCAN1 << 6);  // [6] Scan select Bit 1
    res |= (0 << 7);  // [7] frame type ( 0 - configuration)
    return res;
}
std::optional<uint16_t> MAX11615::GetAdcRead(uint8_t channel) {
    auto res = this->i2c_.WriteRead(MAX11615_ADDRESS, 2, GetConfigData(channel));
    if (!res.has_value()) {
        return;
    }
    uint16_t data = ((res.value()[0] & 0x000f) << 8);
    data += (res.value()[1] & 0x00ff);
    return data;
}

}  // namespace i2c
}  // namespace simba

