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
#include "core/logger/Logger.h"

namespace simba {
namespace i2c {

namespace {
    const constexpr uint8_t HDR_SIZE = 0x03;
}

std::vector<uint8_t> I2CFactory::GetBuffer(std::shared_ptr<Header> header, const std::vector<uint8_t>& payload) {
    header->SetPaylaodSize(payload.size());
    std::vector<uint8_t> buffer = header->GetBuffor();
    buffer.insert(buffer.end(), payload.begin(), payload.end());
    AppLogger::Warning("size4:"+std::to_string(buffer.size())+ "payload_size:"+std::to_string(header->GetPayloadSize()));
    return buffer;
}

std::shared_ptr<Header> I2CFactory::GetHeader(const std::vector<uint8_t>& raw_data) {
    if (raw_data.size() < HDR_SIZE) {
        return nullptr;
    }
    Header hdr(ACTION::Write, 0);
    hdr.SetBuffor(raw_data);
    return std::make_shared<Header>(hdr);
}

std::optional<std::vector<uint8_t>> I2CFactory::GetPayload(const std::vector<uint8_t>& raw_data) {
    if (raw_data.size() <= HDR_SIZE) {
        return {};
    }
    AppLogger::Warning("size22:" + std::to_string(raw_data.size()));
    auto hdr = GetHeader(raw_data);
    AppLogger::Warning("payload size"+std::to_string(hdr->GetPayloadSize()));  // NIE USTAWIA PAYLOAD SIZE
    std::vector<uint8_t> vec(raw_data.begin() + HDR_SIZE, raw_data.end());
    AppLogger::Warning("size23:" + std::to_string(vec.size()));
    return vec;
}

}  // namespace i2c
}  // namespace simba
