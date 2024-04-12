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
#include "core/logger/Logger.h"

namespace simba {
namespace i2c {
void I2CController::ReceiveCallback(const std::string& ip,
  const std::uint16_t& port, const std::vector<std::uint8_t> data) {
    auto hdr = factory_.GetHeader(data);
    if (hdr->GetAction() != ACTION::RES || hdr->GetServiceId() != this->service_id) {
        return;
    }
    this->received_data_ = factory_.GetPayload(data);
    this->data_condition.notify_one();
}

I2CController::I2CController() {
    this->sock_.Init({"SIMBA.I2C.RESPONSE." + std::to_string(this->service_id), 0, 0});
    this->sock_.SetRXCallback(std::bind(&I2CController::ReceiveCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    this->sock_.StartRXThread();
}
core::ErrorCode I2CController::Write(const uint8_t address, const std::vector<uint8_t> data) {
    AppLogger::Debug("Sent i2c msg");
    auto hdr = Header(ACTION::Write, address, this->service_id);
    auto buf = factory_.GetBuffer(std::make_shared<Header>(hdr), data);
    return this->sock_.Transmit("SIMBA.I2C", 0, buf);
}
std::vector<uint8_t> I2CController::Read(const uint8_t address, const std::vector<uint8_t> reg) {
  std::unique_lock<std::mutex> lock(this->mtx);
  Header header(ACTION::Read, address, this->service_id);
  this->sock_.Transmit("SIMBA.I2C", 0, header.GetBuffor());
  this->data_condition.wait(lock, [this] { return this->received_data_.size() > 0; });
  auto data = this->received_data_;
  this->received_data_ = {};
  return this->received_data_;
}
std::optional<uint8_t> I2CController::Read(const uint8_t address, const uint8_t reg) {

}
}  // namespace i2c
}  // namespace simba
