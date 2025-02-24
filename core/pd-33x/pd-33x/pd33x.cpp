/**
 * @file pd33x.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "core/pd-33x/pd-33x/pd33x.hpp"

namespace srp {
namespace core {
namespace {
    constexpr uint8_t kFunction_code = 0x03;
    constexpr uint16_t kP1_start_addr = 0x02;
    constexpr uint16_t kT1_start_addr = 0x08;
    constexpr uint16_t kQuantity = 0x02;
}
bool PD33X::Init(const RS485_conf_t& config, const uint8_t slave_id, std::unique_ptr<MODBUS> modbus) {
    this->modbus_ = std::move(modbus);
    this->modbus_->Init(config, slave_id);
}
std::optional<float> PD33X::ReadP1() {
    auto res = this->modbus_->SendRequest(kFunction_code, kP1_start_addr, kQuantity);
    if (!res.has_value()) {
        return std::nullopt;
    }
    return parseFloat(res.value());
}
std::optional<float> PD33X::ReadT1() {
    auto res = this->modbus_->SendRequest(kFunction_code, kT1_start_addr, kQuantity);
    if (!res.has_value()) {
        return std::nullopt;
    }
    return parseFloat(res.value());
}
float PD33X::parseFloat(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> reorderedData = {data[6], data[5], data[4], data[3]};
    float value;
    std::memcpy(&value, reorderedData.data(), sizeof(float));
    return value;
}

}  // namespace core
}  // namespace srp
