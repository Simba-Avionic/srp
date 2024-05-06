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
    const constexpr char* I2C_IPC_ADDR = "SIMBA.I2C";
    const constexpr char* I2C_RES_IPC_ADDR = "SIMBA.I2C.";
}

void I2CService::RxCallback(const std::string& ip,
  const std::uint16_t& port, const std::vector<std::uint8_t> data) {
    std::unique_lock<std::mutex> lock(this->i2c_mtx);
    std::shared_ptr<i2c::Header> headerPtr = i2c::I2CFactory::GetHeader(data);
    this->i2c_.Ioctl(headerPtr->GetAddress());
    std::vector<uint8_t> payload = i2c::I2CFactory::GetPayload(data);
    std::vector<uint8_t> i2cRes;
    AppLogger::Debug("Receive i2c msg");
    switch (headerPtr->GetAction()) {
    case i2c::ACTION::Write:
      i2c_.Write(payload);
      break;
    case i2c::ACTION::PageWrite:
      i2c_.PageWrite(payload);
      break;
    case i2c::ACTION::Read:
      break;
    case i2c::ACTION::ReadWrite:
      AppLogger::Warning("Receive READ WRITE request"+std::to_string(static_cast<int>(headerPtr->GetPayloadSize())));
      if (payload.size() < 2 || payload.size()%2 != 0) {
        this->sock_.Transmit(I2C_RES_IPC_ADDR+std::to_string(headerPtr->GetServiceId()), 0, {0});
        return;
      }
      for (size_t i = 0; i < payload.size() - 1; i+=2) {
        i2cRes = i2c_.ReadWrite(payload[i], payload[i+1]);
        if (i2cRes.size() != payload[i+1]) {
        this->sock_.Transmit(I2C_RES_IPC_ADDR+std::to_string(headerPtr->GetServiceId()), 0, {0});
        return;
      }
      }
      AppLogger::Warning("Send response to " +std::to_string(headerPtr->GetServiceId()));
      this->sock_.Transmit(I2C_RES_IPC_ADDR+std::to_string(headerPtr->GetServiceId()), 0, i2cRes);
      break;
    case i2c::ACTION::PageRead:
      break;
    default:
      AppLogger::Warning("Invalid type");
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
    this->sock_.Init(com::soc::SocketConfig(I2C_IPC_ADDR, 0, 0));
    this->sock_.SetRXCallback(std::bind(&I2CService::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    this->sock_.StartRXThread();
    return core::ErrorCode::kOk;
}

}  // namespace mw
}  // namespace simba
