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
    const constexpr char* I2C_MY_IPC = "SIMBA.I2C.";
}

void I2CController::Init(uint16_t service_id) {
    this->service_id = service_id;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    AppLogger::Warning(I2C_MY_IPC+std::to_string(service_id));
    this->sock_.Init(com::soc::SocketConfig(I2C_MY_IPC+std::to_string(service_id), 0, 0));
}

core::ErrorCode I2CController::Write(const uint8_t address, const std::vector<uint8_t> data) {
    auto hdr = Header(ACTION::Write, address, this->service_id);
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(hdr), data);
    auto res = this->sock_.Transmit(I2C_IPC, 0, buf);
    if (res.has_value()) {
        return core::ErrorCode::kOk;
    }
    return core::ErrorCode::kConnectionError;
}
core::ErrorCode I2CController::PageWrite(const uint8_t address, const std::vector<uint8_t> data) {
    auto hdr = Header(ACTION::PageWrite, address, this->service_id);
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(hdr), data);
    auto res = this->sock_.Transmit(I2C_IPC, 0, buf);
    if (res.has_value()) {
        return core::ErrorCode::kOk;
    }
    return core::ErrorCode::kConnectionError;
}


std::optional<std::vector<uint8_t>> I2CController::Read(const uint8_t address, const uint8_t reg, const uint8_t size) {
    std::unique_lock<std::mutex> lock(mtx_);
    auto hdr = std::make_shared<Header>(ACTION::Read, address, this->service_id);
    auto buf = I2CFactory::GetBuffer(hdr, {reg, size});
    return this->sock_.Transmit(I2C_IPC, 0, buf);
}

std::optional<std::vector<uint8_t>> I2CController::WriteRead(const uint8_t address, const uint8_t ReadReg,
                                const uint8_t ReadSize, const uint8_t WriteReg, const uint8_t WriteData) {
    std::unique_lock<std::mutex> lock(mtx_);
    auto hdr = std::make_shared<Header>(ACTION::WriteRead, address, this->service_id);
    auto buf = I2CFactory::GetBuffer(hdr, {WriteReg, WriteData, ReadReg, ReadSize});
    return this->sock_.Transmit(I2C_IPC, 0, buf);
}



}  // namespace i2c
}  // namespace simba
