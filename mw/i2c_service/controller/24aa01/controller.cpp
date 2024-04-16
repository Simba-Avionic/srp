/**
 * @file controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <unordered_map>
#include <string>
#include <vector>

#include "mw/i2c_service/controller/i2c_controller.h"
#include "mw/i2c_service/controller/24aa01/controller.hpp"
namespace simba {
namespace i2c {

namespace {
    const constexpr uint8_t EEPROM_ADDRESS = 0x50;
}

EEPROM::EEPROM() {}
void EEPROM::Init(uint16_t service_id) {
    this->i2c_.Init(service_id);
}
std::optional<uint8_t> EEPROM::Read(const uint8_t reg) {
    auto data = this->i2c_.Read(EEPROM_ADDRESS, reg);
    if (!data.has_value()) {
        return {};
    }
    return data.value();
}

std::vector<uint8_t> EEPROM::PageRead() {
    auto data = this->i2c_.Read(EEPROM_ADDRESS, 0x00);
    if (!data.has_value()) {
        return {};
    }
    //TODO // NOLINT
}

core::ErrorCode EEPROM::PageWrite(std::vector<uint8_t> data) {
    data.insert(data.begin(), static_cast<uint8_t>(data.size()));
    return this->i2c_.PageWrite(EEPROM_ADDRESS, data);
}

}  // namespace i2c
}  // namespace simba

