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

std::vector<uint8_t> I2CFactory::GetBuffer(std::shared_ptr<Header> header, const std::vector<uint8_t>& payload) {
        std::vector<uint8_t> buffer;
        buffer.push_back(static_cast<uint8_t>(header->GetAction()));
        buffer.push_back(header->GetAddress());
        buffer.push_back(header->GetReg());
        buffer.push_back(header->GetServiceId());
        buffer.insert(buffer.end(), payload.begin(), payload.end());
        return buffer;
    }

std::shared_ptr<Header> I2CFactory::GetHeader(const std::vector<uint8_t>& raw_data) {
        if (raw_data.size() < 4) {
            return nullptr;
        }
        return std::make_shared<Header>(static_cast<Header::ACTION>(raw_data[0]), raw_data[1], raw_data[2], raw_data[3]);
    }
std::vector<uint8_t> I2CFactory::GetPayload(const std::vector<uint8_t>& raw_data) {
        if (raw_data.size() <= 4) {
            return {};
        }
        return std::vector<uint8_t>(raw_data.begin() + 4, raw_data.end());
    }
}  // namespace i2c
}  // namespace simba
