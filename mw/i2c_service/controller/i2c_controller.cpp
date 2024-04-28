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

I2CController::I2CController() {
    // this->sock_.Init({"SIMBA.I2C.RESPONSE." + std::to_string(this->service_id), 0, 0});
    // this->sock_.SetRXCallback(std::bind(&I2CController::ReceiveCallback, this, std::placeholders::_1,
    //            std::placeholders::_2, std::placeholders::_3));
    // this->sock_.StartRXThread();
}
core::ErrorCode I2CController::Write(const uint8_t address, const std::vector<uint8_t> data) {
    AppLogger::Debug("Sent i2c msg");
    auto hdr = Header(ACTION::Write, address, this->service_id);
    auto buf = factory_.GetBuffer(std::make_shared<Header>(hdr), data);
    return this->sock_.Transmit("SIMBA.I2C", 0, buf);
}
core::ErrorCode I2CController::PageWrite(const uint8_t address, const std::vector<uint8_t> data) {
    AppLogger::Debug("Sent i2c msg");
    auto hdr = Header(ACTION::PageWrite, address, this->service_id);
    auto buf = factory_.GetBuffer(std::make_shared<Header>(hdr), data);
    return this->sock_.Transmit("SIMBA.I2C", 0, buf);
}

}  // namespace i2c
}  // namespace simba
