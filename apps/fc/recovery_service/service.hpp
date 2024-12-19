/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef APPS_FC_RECOVERY_SERVICE_SERVICE_HPP_
#define APPS_FC_RECOVERY_SERVICE_SERVICE_HPP_

#include <memory>
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "simba/apps/RecoveryServiceSkeleton.h"
#include "ara/com/com_error_domain.h"

namespace simba {
namespace apps {

namespace {  // NOLINT
  constexpr auto kLinecutter_id = 1;
  constexpr auto kRecovery_servo_id = 10;
}
class MyRecoveryServiceSkeleton: public RecoveryServiceSkeleton {
 private:
  std::shared_ptr<i2c::PCA9685> servo_controller;
  std::shared_ptr<gpio::IGPIOController> gpio_controller;

 public:
  explicit MyRecoveryServiceSkeleton(const ara::core::InstanceSpecifier& instance, std::shared_ptr<i2c::PCA9685> servo,
                                        std::shared_ptr<gpio::IGPIOController> gpio): RecoveryServiceSkeleton{instance},
                                        servo_controller(servo), gpio_controller(gpio) {}
  virtual ~MyRecoveryServiceSkeleton() = default;

  simba::apps::RecoveryService::NewParachuteStatusEventEventSkeleton NewParachuteStatusEvent{};

 protected:
  ara::core::Result<bool> OpenReefedParachute() override {
    if (!servo_controller || !gpio_controller) {
      return ara::com::MakeErrorCode(ara::com::ComErrc::kSetHandlerNotSet, "pointer to controller not set");
    }
    if (servo_controller->AutoSetServoPosition(kRecovery_servo_id, 1) != core::ErrorCode::kOk) {
      return ara::com::MakeErrorCode(ara::com::ComErrc::kWrongMethodCallProcessingMode,
                                                            "failed to move servo to open position");
    }
    return true;
  }
  ara::core::Result<bool> UnreefeParachute() override {
    if (!servo_controller || !gpio_controller) {
      return ara::com::MakeErrorCode(ara::com::ComErrc::kSetHandlerNotSet, "pointer to controller not set");
    }
    if (gpio_controller->SetPinValue(kLinecutter_id, 1)!= core::ErrorCode::kOk) {
      return ara::com::MakeErrorCode(ara::com::ComErrc::kWrongMethodCallProcessingMode, "failed to run linecutter");
    }
    return true;
  }
};
}  // namespace apps
}  // namespace simba

#endif  // APPS_FC_RECOVERY_SERVICE_SERVICE_HPP_
