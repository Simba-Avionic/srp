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
#include <map>
#include "mw/i2c_service/service/i2c_service.h"
#include "mw/i2c_service/data/i2c_factory.h"
#include "ara/log/log.h"
#include "core/common/condition.h"
namespace srp {
namespace mw {

namespace {
    const constexpr char* I2C_IPC_ADDR = "SRP.I2C";
}
I2CService::I2CService():
    i2c_logger_(ara::log::LoggingMenager::GetInstance()->CreateLogger("i2c", "", ara::log::LogLevel::kInfo)),did_instance("/srp/mw/i2cMWService/i2c_Write_did"), read_instance("/srp/mw/i2cMWService/i2c_Read_did") {}

core::ErrorCode I2CService::Init(std::shared_ptr<core::i2c::II2CDriver> i2c,
                              std::unique_ptr<srp::com::soc::ISocketStream> socket) {
  if (!socket || !i2c) {
    return core::ErrorCode::kInitializeError;
  }
  this->sock_ = std::move(socket);
  this->i2c_ = std::move(i2c);
  return core::ErrorCode::kOk;
}

std::optional<std::vector<uint8_t>> I2CService::ReadWrite(
                  const std::vector<uint8_t> &payload, std::shared_ptr<i2c::Header> headerPtr) {
  i2c_logger_.LogDebug() << ("Receive READ request");
  if (payload.size()%2 != 0) {
    i2c_logger_.LogWarn() << ("Invalid payload size");
    return std::nullopt;
  }
  return i2c_->ReadWrite(payload[0]);
}
std::optional<std::vector<uint8_t>> I2CService::WriteRead(const std::vector<uint8_t> &payload,
                                                          std::shared_ptr<i2c::Header> headerPtr) {
  /*  format payload
   *  size data to read (uint8_t)
   *  data to write (0-255B)
   */
  i2c_logger_.LogDebug() << ("Receive Write Read request");
  if (payload.size() != 2) {
    return std::nullopt;
  }
  uint8_t dataToWrite = payload[1];
  auto res = this->i2c_->Write({dataToWrite});
  if (res != core::ErrorCode::kOk) {
    return std::nullopt;
  }
  return this->i2c_->Read(2);
}

std::vector<uint8_t> I2CService::ActionLogic(const std::shared_ptr<srp::i2c::Header> headerPtr,
                                                      std::optional<std::vector<uint8_t>> payload) {
  if (headerPtr->GetAction() == i2c::ACTION::kWrite) {
      if (payload.value().size() == 1) {
        if (i2c_->Write(payload.value()[0]) == core::ErrorCode::kOk) {
          return {1};
        }
      } else {
        if (i2c_->Write(payload.value()) == core::ErrorCode::kOk) {
          return {1};
        }
      }
      return {};

    } else if (headerPtr->GetAction() == i2c::ACTION::kPageWrite) {
      if (i2c_->PageWrite(payload.value()) == core::ErrorCode::kOk) {
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


std::vector<uint8_t> I2CService::RxCallback(const std::string& ip, const std::uint16_t& port,
                                         const std::vector<std::uint8_t> data) {
    std::unique_lock<std::mutex> lock(this->i2c_mtx);
    auto headerPtr = i2c::I2CFactory::GetHeader(data);
    if (!headerPtr) {
      return {};
    }
    if (this->i2c_->Ioctl(headerPtr->GetAddress()) != core::ErrorCode::kOk) {
      return {};
    }
    auto payload = i2c::I2CFactory::GetPayload(data);
    if (!payload.has_value()) {
      return {};
    }
    return ActionLogic(headerPtr, payload);
}

int I2CService::Run(const std::stop_token& token) {
    core::condition::wait(token);
    this->sock_->StopRXThread();
    pin_did_->StopOffer();
    read_did_->StopOffer();
    return core::ErrorCode::kOk;
}
int I2CService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
    this->Init(std::make_shared<core::i2c::I2CDriver>(), std::make_unique<com::soc::StreamIpcSocket>());
    if (this->i2c_->Init() != core::ErrorCode::kOk) {
      i2c_logger_.LogWarn() << ("Cant init i2c");
      return core::ErrorCode::kInitializeError;
    }

    if (this->sock_->Init(com::soc::SocketConfig(I2C_IPC_ADDR, 0, 0)) != core::ErrorCode::kOk) {
      i2c_logger_.LogWarn() << ("Cant init sock");
      return core::ErrorCode::kInitializeError;
    }
    this->sock_->SetRXCallback(std::bind(&I2CService::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    this->sock_->StartRXThread();

    pin_did_ = std::make_unique<I2CMWWRITE>(this->did_instance, this->i2c_);
    read_did_ = std::make_unique<I2CMWREAD>(this->read_instance, this, this->i2c_);
    pin_did_->Offer();
    read_did_->Offer();
    return core::ErrorCode::kOk;
}

}  // namespace mw
}  // namespace srp
