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
std::vector<uint8_t> EEPROM::Read() {
    auto data = this->i2c_.Read(EEPROM_ADDRESS, 0x00);
    if (!data.has_value()) {
        return {};
    }
    std::vector<uint8_t> regs;
    for (int i = 0x01; i < data.value(); i++) {
        regs.push_back(i);
    }
    return this->i2c_.Read(EEPROM_ADDRESS, regs);
}

std::vector<uint8_t> EEPROM::PageRead(std::vector<uint8_t> reg) {}
core::ErrorCode EEPROM::Write(std::vector<uint8_t> data) {
    uint8_t req = 0x00;
    std::vector<uint8_t> toSend;
    toSend.push_back(data.size());
    for (auto &d : data) {
        toSend.push_back(req);
        toSend.push_back(d);
        req += 1;
    }
    return this->i2c_.Write(EEPROM_ADDRESS, toSend);
}
core::ErrorCode EEPROM::PageWrite(std::vector<uint8_t> data) {}

}  // namespace i2c
}  // namespace simba

