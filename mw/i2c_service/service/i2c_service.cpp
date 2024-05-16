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
#include "mw/i2c_service/service/i2c_service.h"
#include "mw/i2c_service/data/i2c_factory.h"
#include "core/logger/Logger.h"
namespace simba {
namespace mw {

namespace {
    const constexpr char* I2C_IPC_ADDR = "SIMBA.I2C";
}

std::optional<std::vector<uint8_t>> I2CService::ReadWrite(
                  const std::vector<uint8_t> &payload, std::shared_ptr<i2c::Header> headerPtr) {
  AppLogger::Debug("Receive READ WRITE request from:"+std::to_string(static_cast<int>(headerPtr->GetServiceId())));
      if (payload.size()%2 != 0) {
        AppLogger::Warning("Invalid payload size");
        return {};
      }
      return i2c_.ReadWrite(payload[0], payload[1]);
}

std::vector<uint8_t> I2CService::RxCallback(const std::string& ip, const std::uint16_t& port,
                                         std::vector<std::uint8_t> data) {
    std::unique_lock<std::mutex> lock(this->i2c_mtx);
    std::shared_ptr<i2c::Header> headerPtr = i2c::I2CFactory::GetHeader(data);
    if (!headerPtr) {
      return {};
    }
    AppLogger::Debug("ServiceID:"+ std::to_string(static_cast<int>(headerPtr->GetServiceId()))+
                          "Address:"+std::to_string(static_cast<int>(headerPtr->GetAddress())));
    if (this->i2c_.Ioctl(headerPtr->GetAddress()) != core::ErrorCode::kOk) {
      return {};
    }
    auto payload = i2c::I2CFactory::GetPayload(data);
    if (!payload.has_value()) {
      return {};
    }


    if (headerPtr->GetAction() == i2c::ACTION::Write) {
      if (i2c_.Write(payload.value()) == core::ErrorCode::kOk) {
        return {1};
      }
      return {};

    } else if (headerPtr->GetAction() == i2c::ACTION::PageWrite) {
      if (i2c_.PageWrite(payload.value()) == core::ErrorCode::kOk) {
        return {1};
      }
      return {};

    } else if (headerPtr->GetAction() == i2c::ACTION::ReadWrite) {
      auto res = this->ReadWrite(payload.value(), headerPtr);
      auto hdr = i2c::Header(i2c::ACTION::RES, headerPtr->GetAddress(),
                    headerPtr->GetServiceId());
      if (!res.has_value()) {
        return {};
      }
      AppLogger::Debug("Send response to " +std::to_string(headerPtr->GetServiceId())+
                ",payload size:"+std::to_string(res.value().size()));
      hdr.SetPaylaodSize(res.value().size());
      return res.value();
    }
    return {};
}

core::ErrorCode I2CService::Run(std::stop_token token) {
    this->SleepMainThread();
    return core::ErrorCode::kOk;
}
core::ErrorCode I2CService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    if (this->i2c_.Init() != core::ErrorCode::kOk) {
      AppLogger::Warning("Cant init i2c");
      return core::ErrorCode::kInitializeError;
    }
    if (this->sock_.Init(com::soc::SocketConfig(I2C_IPC_ADDR, 0, 0)) != core::ErrorCode::kOk) {
      AppLogger::Warning("Cant init sock");
      return core::ErrorCode::kInitializeError;
    }
    this->sock_.SetRXCallback(std::bind(&I2CService::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    this->sock_.StartRXThread();
    return core::ErrorCode::kOk;
}

}  // namespace mw
}  // namespace simba
