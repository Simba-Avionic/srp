/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_SERVOSERVICE_SERVICE_HPP_
#define APPS_EC_SERVOSERVICE_SERVICE_HPP_
#include <memory>

#include "srp/apps/ServoServiceSkeleton.h"
#include "mw/i2c_service/controller/pca9685/controller.hpp"

namespace srp {
namespace apps {

class MyServoService : public ServoServiceSkeleton {
 private:
  std::shared_ptr<i2c::PCA9685> servo_;

 public:
  MyServoService(const ara::core::InstanceSpecifier& instance, std::shared_ptr<i2c::PCA9685> servo_controller):
      ServoServiceSkeleton{instance}, servo_(servo_controller) {ServoStatusEvent.SetCallback(
      std::bind(&MyServoService::HandleEvent, this, std::placeholders::_1, std::placeholders::_2));
      ServoVentStatusEvent.SetCallback(std::bind(&MyServoService::HandleEvent,
      this, std::placeholders::_1, std::placeholders::_2));
  }

 protected:
  ara::core::Result<bool> SetMainServoValue(const std::uint8_t& in_parm) override {
    if (!(in_parm == 1 || in_parm == 0)) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kFieldValueIsNotValid, "Value need to be 0 or 1");
    }
    auto res = servo_->AutoSetServoPosition(60, in_parm);
    if (res != core::ErrorCode::kOk) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure, "Unknown error from mw::ServoService");
    }
    return ara::core::Result<bool>(true);
  }
  ara::core::Result<std::uint8_t> ReadMainServoValue() override {
    auto res = servo_->ReadServoPosition(60);
    if (!res.has_value()) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure, "Unknown error from mw::ServoService");
    }
    return ara::core::Result<std::uint8_t>(res.value());
  }
  ara::core::Result<bool> SetVentServoValue(const std::uint8_t& in_parm) override {
        if (!(in_parm == 1 || in_parm == 0)) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kFieldValueIsNotValid, "Value need to be 0 or 1");
    }
    auto res = servo_->AutoSetServoPosition(61, in_parm);
    if (res != core::ErrorCode::kOk) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure, "Unknown error from mw::ServoService");
    }
    return ara::core::Result<bool>(true);
  }
  ara::core::Result<std::uint8_t> ReadVentServoValue() override {
    auto res = servo_->ReadServoPosition(61);
    if (!res.has_value()) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure, "Unknown error from mw::ServoService");
    }
    return ara::core::Result<std::uint8_t>(res.value());
  }
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_SERVOSERVICE_SERVICE_HPP_
