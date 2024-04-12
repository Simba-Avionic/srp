/**
 * @file i2c_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <utility>
#include "mw/i2c_service/i2c_service.h"
#include "mw/i2c_service/data/i2c_factory.h"
#include "core/logger/Logger.h"
namespace simba {
namespace mw {

namespace {
    constexpr uint8_t PCA9685_ADDRESS = 0x70;
    constexpr uint8_t MODE1_REG = 0x00;
    constexpr uint8_t PRESCALE_REG = 0xFE;
    constexpr uint8_t LED0_ON_L = 0x06;
    constexpr uint8_t LED0_ON_H = 0x07;
    constexpr uint8_t LED0_OFF_L = 0x08;
    constexpr uint8_t LED0_OFF_H = 0x09;
}

void I2CService::RxCallback(const std::string& ip,
  const std::uint16_t& port, const std::vector<std::uint8_t> data) {
    std::unique_lock<std::mutex> lock(this->i2c_mtx);
    AppLogger::Debug("Receive i2c msg");
    std::shared_ptr<i2c::Header> headerPtr = i2c::I2CFactory::GetHeader(data);
    this->i2c_.Ioctl(headerPtr->GetAddress());
    std::vector<uint8_t> payload = i2c::I2CFactory::GetPayload(data);
    std::optional<std::vector<uint8_t>> result;
    switch (headerPtr->GetAction()) {
      case i2c::ACTION::Write:
        payload = i2c::I2CFactory::GetPayload(data);
        if (payload.size() <= 0) {
          return;
        }
        i2c_.Write(payload);
        break;
      case i2c::ACTION::Read:
        result = i2c_.Read(headerPtr->GetAddress(), payload);
        if (result.has_value()) {
           sock_.Transmit("SIMBA.I2C.RESPONSE." + std::to_string(headerPtr->GetServiceId()), 0, result.value());
        } else {
           sock_.Transmit("SIMBA.I2C.RESPONSE." + std::to_string(headerPtr->GetServiceId()), 0, {});
        }
      case i2c::ACTION::ReadWrite:
      // TODO  // NOLINT
      break;
      default:
      break;
    }
}

core::ErrorCode I2CService::Run(std::stop_token token) {
    this->SleepMainThread();
    return core::ErrorCode::kOk;
}
core::ErrorCode I2CService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    this->i2c_.Init();
    this->sock_.Init(com::soc::SocketConfig("SIMBA.I2C", 0, 0));
    this->sock_.SetRXCallback(std::bind(&I2CService::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    this->sock_.StartRXThread();
    return core::ErrorCode::kOk;
}

}  // namespace mw
}  // namespace simba
