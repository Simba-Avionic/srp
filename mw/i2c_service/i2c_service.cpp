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
    AppLogger::Debug("Receive i2c msg");
    std::unique_lock<std::mutex> lock(this->i2c_mtx);
    std::shared_ptr<i2c::Header> headerPtr = i2c::I2CFactory::GetHeader(data);
    AppLogger::Warning("Address:"+std::to_string(static_cast<int>(headerPtr->GetAddress())));
    AppLogger::Warning("ServiceID:"+std::to_string(static_cast<int>(headerPtr->GetServiceId())));
    if (this->i2c_.Ioctl(headerPtr->GetAddress()) != core::ErrorCode::kOk) {
      AppLogger::Warning("Cant ioctl i2c device");
      return;
    }
    std::optional<std::vector<uint8_t>> payload = i2c::I2CFactory::GetPayload(data);
    if (headerPtr->GetAction() == i2c::ACTION::Write) {
      if (payload.has_value()) {
        i2c_.Write(payload.value());
      }
      return;
    } else if (headerPtr->GetAction() == i2c::ACTION::PageWrite) {
      if (payload.has_value()) {
        i2c_.PageWrite(payload.value());
      }
      return;
    } else if (headerPtr->GetAction() == i2c::ACTION::ReadWrite) {
      if (payload.has_value()) {
        AppLogger::Warning("Receive READ WRITE request"+std::to_string(static_cast<int>(headerPtr->GetPayloadSize())));
      if (payload.value().size() != 2) {
        AppLogger::Warning("first error");
        this->sock_.Transmit(I2C_RES_IPC_ADDR+std::to_string(
              headerPtr->GetServiceId()), 0, i2c::I2CFactory::GetBuffer(std::make_shared<i2c::Header>(i2c::ACTION::RES,
                            headerPtr->GetAddress(), headerPtr->GetServiceId(), headerPtr->GetTransmisionID()), {}));
        return;
      }
      AppLogger::Warning("p1"+std::to_string(payload.value()[0])+",p2:"+std::to_string(payload.value()[1]));
      auto i2cRes = i2c_.ReadWrite(payload.value()[0], payload.value()[1]);
      if (!i2cRes.has_value()) {
        AppLogger::Warning("i2cRes dont have value???????????QOJFNBSIFAHBIHFB(ASFBF)");
        this->sock_.Transmit(I2C_RES_IPC_ADDR+std::to_string(
              headerPtr->GetServiceId()), 0, i2c::I2CFactory::GetBuffer(std::make_shared<i2c::Header>(i2c::ACTION::RES,
                            headerPtr->GetAddress(), headerPtr->GetServiceId(), headerPtr->GetTransmisionID()), {}));
        AppLogger::Warning("second error");
        return;
      }
      AppLogger::Warning("Send response to " +std::to_string(headerPtr->GetServiceId())+
                "size:"+std::to_string(i2cRes.value().size()) +"with payload:"+std::to_string(i2cRes.value()[0]));
      AppLogger::Warning(I2C_RES_IPC_ADDR+std::to_string(headerPtr->GetServiceId()));
      i2c::Header hdr(i2c::ACTION::RES, headerPtr->GetAddress(),
                        headerPtr->GetServiceId(), headerPtr->GetTransmisionID());
      hdr.SetPaylaodSize(i2cRes.value().size());
      this->sock_.Transmit(I2C_RES_IPC_ADDR+std::to_string(headerPtr->GetServiceId()), 0,
                  i2c::I2CFactory::GetBuffer(std::make_shared<i2c::Header>(hdr), i2cRes.value()));
    }
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
