/**
 * @file servoController.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <utility>

#include "servoController.hpp"
#include "mw/i2c_service/pca9685/data/servo_hdr.hpp"

namespace simba {
namespace core {

ServoController::ServoController(std::unique_ptr<com::soc::IpcSocket> sock): sock_(std::move(sock)) {}

ServoController::ServoController() {
    this->sock_ = std::make_unique<com::soc::IpcSocket>();
}

void ServoController::Init(uint16_t app_id) {
    this->app_id = app_id;
}

ErrorCode ServoController::SetServoPos(uint8_t actuatorID, uint8_t position) {
    i2c::ServoHdr hdr{actuatorID, position};
    return this->sock_->Transmit("SIMBA.PCA9685", 0, hdr.GetBuffor());
}

ErrorCode ServoController::MANSetServoPos(uint8_t actuatorID, uint16_t position) {
    i2c::ServoHdr hdr{actuatorID, position, i2c::smode_t::MAN};
    return this->sock_->Transmit("SIMBA.PCA9685", 0, hdr.GetBuffor());
}

}  // namespace core
}  // namespace simba
