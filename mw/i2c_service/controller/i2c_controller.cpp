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
#include <sys/stat.h>
#include <unistd.h>
#include "mw/i2c_service/controller/i2c_controller.h"
#include "core/logger/Logger.h"

namespace simba {
namespace i2c {

namespace {
    const constexpr char* I2C_IPC = "SIMBA.I2C";
}


core::ErrorCode I2CController::Write(const uint8_t address, const std::vector<uint8_t> data) {
    AppLogger::Warning("wywołano write z pca9685");
    auto hdr = Header(ACTION::kWrite, address);
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(hdr), data);
    AppLogger::Warning("SIZE::"+std::to_string(buf.size()));
    auto res = this->sock_.Transmit(I2C_IPC, 0, buf);
    if (res.has_value()) {
        return core::ErrorCode::kOk;
    }
    return core::ErrorCode::kConnectionError;
}
core::ErrorCode I2CController::PageWrite(const uint8_t address, const std::vector<uint8_t> data) {
    auto hdr = Header(ACTION::kPageWrite, address);
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(hdr), data);
    auto res = this->sock_.Transmit(I2C_IPC, 0, buf);
    if (res.has_value()) {
        return core::ErrorCode::kOk;
    }
    return core::ErrorCode::kConnectionError;
}


std::optional<std::vector<uint8_t>> I2CController::Read(const uint8_t address, const uint8_t reg, const uint8_t size) {
    std::unique_lock<std::mutex> lock(mtx_);
    auto hdr = std::make_shared<Header>(ACTION::kRead, address);
    auto buf = I2CFactory::GetBuffer(hdr, {reg, size});
    return this->sock_.Transmit(I2C_IPC, 0, buf);
}

std::optional<std::vector<uint8_t>> I2CController::WriteRead(const uint8_t address,
                                                        const uint8_t WriteData, const uint8_t ReadSize) {
    std::unique_lock<std::mutex> lock(mtx_);
    auto hdr = std::make_shared<Header>(ACTION::kWriteRead, address);
    auto buf = I2CFactory::GetBuffer(hdr, {ReadSize, WriteData});
    return this->sock_.Transmit(I2C_IPC, 0, buf);
}



}  // namespace i2c
}  // namespace simba
