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
#include "i2c_controller.h"
#include "core/logger/Logger.h"

namespace simba {
namespace i2c {
I2CController::I2CController() {
    // this->sock_.Init({"SIMBA.I2C.RESPONSE." + std::to_string(this->service_id), 0, 0});
    // this->sock_.SetRXCallback(std::bind(&I2CController::ReceiveCallback, this, std::placeholders::_1,
    //            std::placeholders::_2, std::placeholders::_3));
    // this->sock_.StartRXThread();
}
core::ErrorCode I2CController::Write(const uint8_t address, const std::vector<uint8_t> data) {
    AppLogger::Warning("Sent i2c msg");
    std::lock_guard<std::mutex> lock(this->mtx);
    auto hdr = Header(ACTION::Write, address, this->service_id);
    std::vector<uint8_t> convertedVector;
    convertedVector.reserve(data.size() * 2);
    auto buf = factory_.GetBuffer(std::make_shared<Header>(hdr), data);
    return this->sock_.Transmit("SIMBA.I2C", 0, buf);
}
std::vector<uint8_t> I2CController::Read(const uint8_t address, const uint8_t reg) {
    std::lock_guard<std::mutex> lock(this->mtx);
    // std::unique_lock<std::mutex> lock(this->data_mutex);
    // this->data_condition.wait(lock, [this] { return this->data_ready; });
    // Header header(Header::ACTION::Read, address, reg, this->service_id);
    // this->sock_.Transmit("SIMBA.I2C", 0, header.GetBuffor());
    // this->data_ready = false;
    // this->mtx.unlock();
    // return this->received_data_;
}
void I2CController::ReceiveCallback(const std::string& ip,
  const std::uint16_t& port, const std::vector<std::uint8_t> data)  {
    this->received_data_ = data;
    this->data_ready = true;
    this->data_condition.notify_one();
}
}  // namespace i2c
}  // namespace simba
