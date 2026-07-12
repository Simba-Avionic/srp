/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Service implementation for secondary EC ethanol valve control
 * @version 0.1
 * @date 2026-07-01
 *
 * @copyright Copyright (c) 2026
 */
#ifndef APPS_SEC_EC_SERVOSERVICE_SERVICE_HPP_
#define APPS_SEC_EC_SERVOSERVICE_SERVICE_HPP_

#include <memory>
#include <cstdint>
#include <utility>

#include "srp/apps/SecServoServiceSkeleton.h"
#include "apps/ec/ServoService/servoController/servo_controller.hpp"
#include "ara/com/com_error_domain.h"

namespace srp {
namespace apps {

namespace {  // NOLINT
  static constexpr auto kEthanolMainValveID = 63;
  static constexpr auto kEthanolVentValveID = 64;
}

class MySecServoService : public SecServoServiceSkeleton {
 private:
  const std::shared_ptr<service::ServoController> servo_;

  ara::core::Result<bool> SetServoInternal(std::uint8_t servo_id, std::uint8_t value) {
    if (!servo_) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure,
                                       "Servo controller is not initialized");
    }
    if (value > 2) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kFieldValueIsNotValid,
                                       "Value must be 0 or 1");
    }
    auto res = servo_->AutoSetServoPosition(servo_id, value);
    if (!res) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure,
                                       "Unknown error from mw::ServoService");
    }
    return true;
  }

 public:
  MySecServoService(const ara::core::InstanceSpecifier& instance,
                    std::shared_ptr<service::ServoController> servo_controller)
      : SecServoServiceSkeleton{instance}, servo_(std::move(servo_controller)) {
  }

 protected:
  ara::core::Result<bool> SetEtanolMainValve(const std::uint8_t& in_parm) override {
    return SetServoInternal(kEthanolMainValveID, in_parm);
  }
  ara::core::Result<bool> SetEthanolVentValve(const std::uint8_t& in_parm) override {
    return SetServoInternal(kEthanolVentValveID, in_parm);
  }
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_SEC_EC_SERVOSERVICE_SERVICE_HPP_
