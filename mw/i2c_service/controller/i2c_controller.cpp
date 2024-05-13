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
    auto res = this->sock_.Init(com::soc::SocketConfig(I2C_MY_IPC+std::to_string(service_id), 0, 0));
    if (res != core::ErrorCode::kOk) {
        AppLogger::Warning("5 error");
    }
    this->sock_.SetRXCallback(
          std::bind(&I2CController::RXCallback, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));
    this->sock_.StartRXThread();
}

core::ErrorCode I2CController::Write(const uint8_t address, const std::vector<uint8_t> data) {
    AppLogger::Debug("Sent i2c msg");
    auto hdr = Header(ACTION::Write, address, this->service_id);
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(hdr), data);
    return this->sock_.Transmit(I2C_IPC, 0, buf);
}
core::ErrorCode I2CController::PageWrite(const uint8_t address, const std::vector<uint8_t> data) {
    AppLogger::Debug("Sent i2c msg");
    auto hdr = Header(ACTION::PageWrite, address, this->service_id);
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(hdr), data);
    return this->sock_.Transmit(I2C_IPC, 0, buf);
}

void I2CController::RXCallback(const std::string& ip, const std::uint16_t& port,
                            std::vector<std::uint8_t> data) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto hdr  = i2c::I2CFactory::GetHeader(data);
    AppLogger::Warning("transmissionID:"+std::to_string(hdr->GetTransmisionID())+":"+std::to_string(this->transmission_id));
    if (hdr->GetTransmisionID() == this->transmission_id) {
        response_hdr = std::move(hdr);
        response_payload = i2c::I2CFactory::GetPayload(data);
        response_received_ = true;
        cv_.notify_one();
    } else {
        AppLogger::Warning("Error: Incorrect transmission ID received.");
    }
}

std::optional<std::vector<uint8_t>> I2CController::Read(const uint8_t address, const uint8_t reg, const uint8_t size) {
    std::unique_lock<std::mutex> lock(mtx_);
    auto current_transmission_id = this->transmission_id;
    AppLogger::Warning("Sent i2c msg");
    auto buf = I2CFactory::GetBuffer(
                std::make_shared<Header>(ACTION::ReadWrite,
                            address, this->service_id, current_transmission_id), {reg, size});
    this->sock_.Transmit(I2C_IPC, 0, buf);
    if (cv_.wait_for(lock, std::chrono::seconds(3), [this, current_transmission_id] {
        return response_received_ && response_hdr->GetTransmisionID() == current_transmission_id;
    })) {
        AppLogger::Warning("Received: " +
                std::to_string(response_hdr->GetTransmisionID()) + ":" + std::to_string(current_transmission_id));
        if (response_received_ && response_hdr->GetTransmisionID() == current_transmission_id) {
            response_received_ = false;
            this->transmission_id++;
            return response_payload;
        }
    } else {
        AppLogger::Warning("Error: Timeout occurred.");
    }
    this->transmission_id++;
    AppLogger::Warning("Error: Incorrect response received.");
    return {};
}



}  // namespace i2c
}  // namespace simba
