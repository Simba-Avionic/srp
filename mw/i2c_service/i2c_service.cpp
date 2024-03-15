/**
 * @file i2c_service.cc
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "i2c_service.h"
namespace simba {
namespace mw {

void I2CService::I2CRxCallback(const std::string& ip,
  const std::uint16_t& port, const std::vector<std::uint8_t> data) {
    if (data.size() <= 0) {
        return;
    }
    std::shared_ptr<i2c::Header> headerPtr = i2c::I2CFactory::GetHeader(data);
    uint8_t action = headerPtr->GetAction();
    uint8_t address = headerPtr->GetAddress();
    uint8_t reg = headerPtr->GetReg();
    uint8_t service_id = headerPtr->GetServiceId();
    if (action == i2c::Header::ACTION::Write) {
      std::vector<uint8_t> payload = i2c::I2CFactory::GetPayload(data);
      if (payload.size() <= 0) {
        return;
      }
      i2c_driver_.Write(address, reg, payload);
    }
    if (action == i2c::Header::ACTION::Read) {
      std::optional<std::vector<uint8_t>> result = i2c_driver_.Read(address, reg);
      if (result.has_value()) {
        i2c_sock_.Transmit("SIMBA.I2C.RESPONSE." + std::to_string(service_id), 0, result.value());
      } else {
        i2c_sock_.Transmit("SIMBA.I2C.RESPONSE." + std::to_string(service_id), 0, {});
      }
    }
}
core::ErrorCode I2CService::Run(std::stop_token token) {
    this->SleepMainThread();
    return core::ErrorCode::kOk;
}
core::ErrorCode I2CService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
        this->i2c_sock_.Init({"SIMBA.I2C", 0, 0});
        this->i2c_sock_.SetRXCallback(std::bind(&I2CService::I2CRxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
        if (this->i2c_driver_.init("/dev/i2c-1") != core::ErrorCode::kOk) {
          return core::ErrorCode::kError;
        }
        this->i2c_sock_.StartRXThread();
    return core::ErrorCode::kOk;
}

}  // namespace mw
}  // namespace simba
