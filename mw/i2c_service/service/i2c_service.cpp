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
#include <algorithm>
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
  AppLogger::Debug("Receive READ request");
      if (payload.size()%2 != 0) {
        AppLogger::Warning("Invalid payload size");
        return {};
      }
      return i2c_.ReadWrite(payload[0], payload[1]);
}
std::optional<std::vector<uint8_t>> I2CService::WriteRead(const std::vector<uint8_t> &payload,
                                                          std::shared_ptr<i2c::Header> headerPtr) {
  /*  format payload
   *  size data to read (uint8_t)
   *  data to write (0-255B)
   */
  AppLogger::Debug("Receive Write Read request");
  if (payload.size() != 2) {
    return {};
  }
  uint8_t readSize = payload[0];
  uint8_t dataToWrite = payload[1];
  auto res = this->i2c_.Write({dataToWrite});
  if (res != core::ErrorCode::kOk) {
    return {};
  }
  return this->i2c_.Read(2);
}


std::vector<uint8_t> I2CService::RxCallback(const std::string& ip, const std::uint16_t& port,
                                         const std::vector<std::uint8_t> data) {
    AppLogger::Warning("size12:"+std::to_string(data.size()));
    std::unique_lock<std::mutex> lock(this->i2c_mtx);
    std::shared_ptr<i2c::Header> headerPtr = i2c::I2CFactory::GetHeader(data);
    AppLogger::Warning("size13:"+std::to_string(data.size()));
    if (!headerPtr) {
      return {};
    }
    if (this->i2c_.Ioctl(headerPtr->GetAddress()) != core::ErrorCode::kOk) {
      return {};
    }
    AppLogger::Warning("size14:"+std::to_string(data.size()));
    auto payload = i2c::I2CFactory::GetPayload(data);
    AppLogger::Warning("size15:"+std::to_string(data.size()));
    if (!payload.has_value()) {
      return {};
    }
    AppLogger::Warning("Receive i2c req with payload size:"+std::to_string(payload.value().size()));
    if (headerPtr->GetAction() == i2c::ACTION::kWrite) {
      AppLogger::Warning("receive write req");
      if (payload.value().size() == 1) {
        if (i2c_.Write(payload.value()[0]) == core::ErrorCode::kOk) {
          return {1};
        }
      } else {
        AppLogger::Warning("1");
        AppLogger::Warning("size2:"+std::to_string(payload.value().size()));
        if (i2c_.Write(payload.value()) == core::ErrorCode::kOk) {
          return {1};
        }
      }
      return {};

    } else if (headerPtr->GetAction() == i2c::ACTION::kPageWrite) {
      if (i2c_.PageWrite(payload.value()) == core::ErrorCode::kOk) {
        return {1};
      }
      return {};

    } else if (headerPtr->GetAction() == i2c::ACTION::kRead) {
      auto res = this->ReadWrite(payload.value(), headerPtr);
      if (!res.has_value()) {
        return {};
      }
      return res.value();
    } else if (headerPtr->GetAction() == i2c::ACTION::kWriteRead) {
      auto res = this->WriteRead(payload.value(), headerPtr);
      if (!res.has_value()) {
        return {};
      }
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
