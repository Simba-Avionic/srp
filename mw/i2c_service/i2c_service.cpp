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
#include "i2c_service.h"
#include "mw/i2c_service/pca9685/data/servo_hdr.hpp"
namespace simba {
namespace mw {

void  I2CService::ServoRxCallback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t> data) {
      AppLogger::Warning("Recive change msg");
      i2c::ServoHdr hdr{0, 0};
      hdr.SetBuffor(data);
      if (hdr.GetMode() == i2c::smode_t::AUTO) {
        this->pca9685_.AutoSetServoPos(hdr.GetActuatorID(), hdr.GetPosition());
      } else {
        this->pca9685_.ManSetServoPos(hdr.GetActuatorID(), hdr.GetPosition());
      }
      AppLogger::Debug("set servo with id:"+std::to_string(
                hdr.GetActuatorID())+"pos:"+std::to_string(hdr.GetPosition()));
}


core::ErrorCode I2CService::Run(std::stop_token token) {
    this->SleepMainThread();
    return core::ErrorCode::kOk;
}
core::ErrorCode I2CService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
        this->servo_sock_.Init({"SIMBA.PCA9685", 0, 0});
        this->servo_sock_.SetRXCallback(std::bind(&I2CService::ServoRxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
        this->servo_sock_.StartRXThread();
        this->pca9685_.Init(this->app_id_);
    return core::ErrorCode::kOk;
}

}  // namespace mw
}  // namespace simba
