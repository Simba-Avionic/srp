/**
 * @file i2c_factory.cpp
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mw/i2c_service/data/i2c_factory.h"

namespace simba {
namespace i2c {

namespace {
    const constexpr uint8_t HDR_SIZE = 0x04;
}

std::vector<uint8_t> I2CFactory::GetBuffer(std::shared_ptr<Header> header, const std::vector<uint8_t>& payload) {
        std::vector<uint8_t> buffer = header->GetBuffor();
        buffer.insert(buffer.end(), payload.begin(), payload.end());
        return buffer;
    }

std::shared_ptr<Header> I2CFactory::GetHeader(const std::vector<uint8_t>& raw_data) {
        if (raw_data.size() < 4) {
            return nullptr;
        }
        Header hdr(i2c::ACTION::Read, 0x00, 0x00);
        hdr.SetBuffor(raw_data);
        return std::make_shared<Header>(hdr);
    }
std::vector<uint8_t> I2CFactory::GetPayload(const std::vector<uint8_t>& raw_data) {
        if (raw_data.size() <= HDR_SIZE) {
            return {};
        }
        return std::vector<uint8_t>(raw_data.begin() + HDR_SIZE, raw_data.end());
    }
}  // namespace i2c
}  // namespace simba
