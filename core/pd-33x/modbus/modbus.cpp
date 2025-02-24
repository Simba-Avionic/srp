/**
 * @file modbus.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "core/pd-33x/modbus/modbus.hpp"
namespace srp {
namespace core {

uint16_t MODBUS::Calculate_CRC16(const std::vector<uint8_t>& data) {
    unsigned int Crc = 0xFFFF;
    unsigned char n, m, x;
    m = data.size();
    x = 0;
    while (m > 0) {
        Crc^= data[x];
        for (n=0; n < 8; n++) {
            if (Crc&1) {
                Crc>>= 1;
                Crc^= 0xA001;
            } else {
                Crc>>= 1;
            }
        }
        m--;
        x++;
    }
    return Crc;
}
bool MODBUS::Validate_CRC16(const std::vector<uint8_t>& data) {
    if (data.size() < 3) {
        return false;
    }
    uint16_t received_crc = data[data.size() - 2] | (data[data.size() - 1] << 8);
    std::vector<uint8_t> payload(data.begin(), data.end() - 2);
    return Calculate_CRC16(payload) == received_crc;
}

bool MODBUS::Init(const RS485_conf_t& config, const uint8_t slave_id,
    std::unique_ptr<core::IRS485> rs485) {
    this->rs485_ = std::move(rs485);
    this->slave_id = slave_id;
    return this->rs485_->Init(config);
}

std::optional<std::vector<uint8_t>> MODBUS::SendRequest(uint8_t function_code, uint16_t start_addr, uint16_t quantity) {
    std::vector<uint8_t> request = {slave_id, function_code,
                                static_cast<uint8_t>(start_addr >> 8), static_cast<uint8_t>(start_addr & 0xFF),
                                static_cast<uint8_t>(quantity >> 8), static_cast<uint8_t>(quantity & 0xFF)};
    uint16_t crc = Calculate_CRC16(request);
    request.push_back(crc & 0xFF);
    request.push_back(crc >> 8);
    return rs485_->WriteRead(request, 5 + quantity * 2);
}

}  // namespace core
}  // namespace srp
