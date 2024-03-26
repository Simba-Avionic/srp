/**
 * @file eeprom_factory.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "eeprom_factory.hpp"

#include <algorithm>

namespace simba {
namespace i2c {

std::vector<uint8_t> EepromMsgFactory::GetBuffer(std::shared_ptr<i2c::EepromHdr> hdr,
                                        std::vector<uint8_t> payload) {
    std::vector<uint8_t> res{hdr->GetBuffor()};
    std::copy(payload.begin(), payload.end(), std::back_inserter(res));
    return res;
}
std::shared_ptr<i2c::EepromHdr> EepromMsgFactory::GetHeader(std::vector<uint8_t> raw_data) {
    auto hdr = std::make_shared<i2c::EepromHdr>(0, 0, i2c::operation_t::READ);
    hdr->SetBuffor(raw_data);
    return hdr;
}

std::vector<uint8_t> EepromMsgFactory::GetPayload(std::vector<uint8_t> raw_data) {
    std::vector<uint8_t> payload;
    if (raw_data.size() > 4) {
        std::copy(raw_data.begin()+0x4, raw_data.end(), std::back_inserter(payload));
        return payload;
    }
    return {};
}

}  // namespace i2c
}  // namespace simba
