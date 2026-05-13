/**
 * @file i2c_controller.cpp
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "mw/i2c_service/controller/i2c_controller.h"

#include <utility>

#include "ara/log/log.h"

namespace srp {
namespace i2c {

namespace {
    const constexpr char* I2C_IPC = "SRP.I2C";
}

core::ErrorCode I2CController::Init(std::unique_ptr<srp::com::soc::ISocketStream> socket) {
    if (!socket) {
        ara::log::LogError() << "I2CController::Init failed: invalid socket";
        return core::ErrorCode::kInitializeError;
    }
    this->sock_ = std::move(socket);
    ara::log::LogInfo() << "I2CController initialized";
    return core::ErrorCode::kOk;
}
core::ErrorCode I2CController::Write(const uint8_t address, const std::vector<uint8_t> data) {
    auto res = SendData(ACTION::kWrite, address, data);
    if (!res.has_value()) {
        return core::ErrorCode::kConnectionError;
    }
    if (res.value().empty()) {
        ara::log::LogDebug() << "I2C write NACK for address " << address;
        return core::ErrorCode::kConnectionError;
    }
    return core::ErrorCode::kOk;
}
core::ErrorCode I2CController::PageWrite(const uint8_t address, const std::vector<uint8_t> data) {
    auto res = SendData(ACTION::kPageWrite, address, data);
    if (!res.has_value()) {
        ara::log::LogDebug() << "I2C page write transport error for address " << address;
        return core::ErrorCode::kConnectionError;
    }
    if (res.value().empty()) {
        ara::log::LogDebug() << "I2C page write NACK for address " << address;
        return core::ErrorCode::kConnectionError;
    }
    return core::ErrorCode::kOk;
}


std::optional<std::vector<uint8_t>> I2CController::Read(const uint8_t address, const uint8_t reg, const uint8_t size) {
    return SendData(ACTION::kRead, address, {reg, size});
}



std::optional<std::vector<uint8_t>> I2CController::WriteRead(const uint8_t address,
                                                        const uint8_t WriteData, const uint8_t ReadSize) {
    return SendData(ACTION::kWriteRead, address, {ReadSize, WriteData});
}

std::optional<std::vector<uint8_t>> I2CController::WriteReadBuffer(
        const uint8_t address, const std::vector<uint8_t>& write_data, const uint8_t read_size) {
    std::vector<uint8_t> payload;
    payload.reserve(write_data.size() + 1);
    payload.push_back(read_size);
    payload.insert(payload.end(), write_data.begin(), write_data.end());
    return SendData(ACTION::kWriteReadBuffer, address, payload);
}


std::optional<std::vector<uint8_t>> I2CController::SendData(
            ACTION action, uint8_t address, const std::vector<uint8_t>& payload) {
    if (!this->sock_) {
        ara::log::LogError() << "I2C SendData failed: socket not initialized";
        return std::nullopt;
    }
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(action, address), payload);
    auto res = this->sock_->Transmit(I2C_IPC, 0, buf);
    if (!res.has_value()) {
        ara::log::LogDebug() << "I2C SendData transport error, action " << action
                             << ", address " << address;
    }
    return res;
}



}  // namespace i2c
}  // namespace srp
