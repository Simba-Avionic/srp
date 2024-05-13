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
    const constexpr char* I2C_RES_IPC_ADDR = "SIMBA.I2C.";
}

std::optional<std::vector<uint8_t>> I2CService::ReadWrite(
                  const std::vector<uint8_t> &payload, std::shared_ptr<i2c::Header> headerPtr) {
  AppLogger::Warning("Receive READ WRITE request"+std::to_string(static_cast<int>(headerPtr->GetPayloadSize())));
      if (payload.size()%2 != 0) {
        AppLogger::Warning("Invalid payload size");
        this->sock_.Transmit(I2C_RES_IPC_ADDR+std::to_string(
              headerPtr->GetServiceId()), 0, i2c::I2CFactory::GetBuffer(std::make_shared<i2c::Header>(i2c::ACTION::RES,
                            headerPtr->GetAddress(), headerPtr->GetServiceId(), headerPtr->GetTransmisionID()), {}));
        return {};
      }
      std::vector<uint8_t> res_data;
      for (uint16_t i = 0; i < payload.size(); i+=2) {
        AppLogger::Warning("reg:"+std::to_string(payload[0])+",size:"+std::to_string(payload[1]));
        auto i2c__data = i2c_.ReadWrite(payload[i], payload[i+1]);
        if (i2c__data.has_value()) {
          res_data.insert(res_data.end(), i2c__data.value().begin(), i2c__data.value().end());
        } else {
          AppLogger::Warning("I2C dont return value for:" +std::to_string(payload[i]));
          return {};
        }
      }
      return res_data;
}

void I2CService::RxCallback(const std::string& ip,
  const std::uint16_t& port, const std::vector<std::uint8_t> data) {
    std::unique_lock<std::mutex> lock(this->i2c_mtx);
    std::shared_ptr<i2c::Header> headerPtr = i2c::I2CFactory::GetHeader(data);
    if (!headerPtr) {
      return;
    }
    AppLogger::Debug("Service:"+ std::to_string(static_cast<int>(headerPtr->GetServiceId()))+
                          "Address:"+std::to_string(static_cast<int>(headerPtr->GetAddress())));
    if (this->i2c_.Ioctl(headerPtr->GetAddress()) != core::ErrorCode::kOk) {
      this->sock_.Transmit(I2C_RES_IPC_ADDR + std::to_string(headerPtr->GetServiceId()), 0,
                                                  i2c::I2CFactory::GetBuffer(headerPtr, {}));
      return;
    }
    std::optional<std::vector<uint8_t>> payload = i2c::I2CFactory::GetPayload(data);
    if (!payload.has_value()) {
      this->sock_.Transmit(I2C_RES_IPC_ADDR + std::to_string(headerPtr->GetServiceId()), 0,
                                                  i2c::I2CFactory::GetBuffer(headerPtr, {}));
      return;
    }
    if (headerPtr->GetAction() == i2c::ACTION::Write) {
      i2c_.Write(payload.value());
      return;
    } else if (headerPtr->GetAction() == i2c::ACTION::PageWrite) {
      i2c_.PageWrite(payload.value());
      return;
    } else if (headerPtr->GetAction() == i2c::ACTION::ReadWrite) {
      auto res = this->ReadWrite(payload.value(), headerPtr);
      headerPtr->SetAction(i2c::ACTION::RES);
      if (!res.has_value()) {
        headerPtr->SetPaylaodSize(0);
        this->sock_.Transmit(I2C_RES_IPC_ADDR + std::to_string(headerPtr->GetServiceId()), 0,
                                                  i2c::I2CFactory::GetBuffer(headerPtr, {}));
      }
      AppLogger::Warning("Send response to " +std::to_string(headerPtr->GetServiceId())+
                "size:"+std::to_string(res.value().size()) +"with payload:"+std::to_string(res.value()[0]));
      headerPtr->SetPaylaodSize(res.value().size());
      this->sock_.Transmit(I2C_RES_IPC_ADDR + std::to_string(headerPtr->GetServiceId()), 0,
                                                  i2c::I2CFactory::GetBuffer(headerPtr, res.value()));
      return;
    }
}

core::ErrorCode I2CService::Run(std::stop_token token) {
    this->SleepMainThread();
    return core::ErrorCode::kOk;
}
core::ErrorCode I2CService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    if (this->i2c_.Init() != core::ErrorCode::kOk) {
      AppLogger::Warning("Cant init i2c");
    }
    this->sock_.Init(com::soc::SocketConfig(I2C_IPC_ADDR, 0, 0));
    this->sock_.SetRXCallback(std::bind(&I2CService::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    this->sock_.StartRXThread();
    return core::ErrorCode::kOk;
}

}  // namespace mw
}  // namespace simba
