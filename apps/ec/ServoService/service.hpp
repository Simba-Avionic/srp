/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Service implementation for Servo control
 * @version 0.2
 * @date 2024-10-29
 * * @copyright Copyright (c) 2025
 */
#ifndef APPS_EC_SERVOSERVICE_SERVICE_HPP_
#define APPS_EC_SERVOSERVICE_SERVICE_HPP_

#include <memory>
#include <cstdint>
#include <utility>

#include "srp/apps/ServoServiceSkeleton.h"
#include "apps/ec/ServoService/servoController/servo_controller.hpp"
#include "ara/com/com_error_domain.h"
#include "core/common/error_code.h"

namespace srp {
namespace apps {

namespace {  // NOLINT
  constexpr std::uint8_t kMainValveID = 60;
  constexpr std::uint8_t kVentValveID = 61;
  constexpr std::uint8_t kDumpValveID = 62;
}

class MyServoService : public ServoServiceSkeleton {
 private:
  const std::shared_ptr<service::ServoController> servo_;
  ara::core::Result<bool> SetServoInternal(std::uint8_t servo_id, std::uint8_t value) {
    if (value > 1) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kFieldValueIsNotValid, "Value must be 0 or 1");
    }
    auto res = servo_->AutoSetServoPosition(servo_id, value);
    if (res != core::ErrorCode::kOk) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure, "Unknown error from mw::ServoService");
    }
    return true;
  }
  ara::core::Result<std::uint8_t> ReadServoInternal(std::uint8_t servo_id) {
    auto res = servo_->ReadServoPosition(servo_id);
    if (!res.has_value()) {
        return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure, "Unknown error from mw::ServoService");
    }
    return res.value();
  }

 public:
  MyServoService(const ara::core::InstanceSpecifier& instance,
                 std::shared_ptr<service::ServoController> servo_controller)
      : ServoServiceSkeleton{instance}, servo_(std::move(servo_controller)) {
  }

 protected:
  ara::core::Result<bool> SetMainServoValue(const std::uint8_t& in_parm) override {
    return SetServoInternal(kMainValveID, in_parm);
  }

  ara::core::Result<std::uint8_t> ReadMainServoValue() override {
    return ReadServoInternal(kMainValveID);
  }

  ara::core::Result<bool> SetVentServoValue(const std::uint8_t& in_parm) override {
    return SetServoInternal(kVentValveID, in_parm);
  }

  ara::core::Result<std::uint8_t> ReadVentServoValue() override {
    return ReadServoInternal(kVentValveID);
  }

  ara::core::Result<bool> SetDumpValue(const std::uint8_t& in_parm) override {
    return SetServoInternal(kDumpValveID, in_parm);
  }

  ara::core::Result<std::uint8_t> ReadDumpValue() override {
    return ReadServoInternal(kDumpValveID);
  }
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_SERVOSERVICE_SERVICE_HPP_
