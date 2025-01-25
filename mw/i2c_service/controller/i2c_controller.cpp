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


namespace srp {
namespace i2c {

namespace {
    const constexpr char* I2C_IPC = "SRP.I2C";
}

core::ErrorCode I2CController::Init(std::unique_ptr<srp::com::soc::ISocketStream> socket) {
    this->sock_ = std::move(socket);
    return core::ErrorCode::kOk;
}
core::ErrorCode I2CController::Write(const uint8_t address, const std::vector<uint8_t> data) {
    auto res = SendData(ACTION::kWrite, address, data);
    if (res.has_value()) {
        return core::ErrorCode::kOk;
    }
    return core::ErrorCode::kConnectionError;
}
core::ErrorCode I2CController::PageWrite(const uint8_t address, const std::vector<uint8_t> data) {
    auto res = SendData(ACTION::kPageWrite, address, data);
    if (res.has_value()) {
        return core::ErrorCode::kOk;
    }
    return core::ErrorCode::kConnectionError;
}


std::optional<std::vector<uint8_t>> I2CController::Read(const uint8_t address, const uint8_t reg, const uint8_t size) {
    return SendData(ACTION::kRead, address, {reg, size});
}



std::optional<std::vector<uint8_t>> I2CController::WriteRead(const uint8_t address,
                                                        const uint8_t WriteData, const uint8_t ReadSize) {
    return SendData(ACTION::kWriteRead, address, {ReadSize, WriteData});
}


std::optional<std::vector<uint8_t>> I2CController::SendData(
            ACTION action, uint8_t address, const std::vector<uint8_t>& payload) {
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(action, address), payload);
    return this->sock_->Transmit(I2C_IPC, 0, buf);
}



}  // namespace i2c
}  // namespace srp
