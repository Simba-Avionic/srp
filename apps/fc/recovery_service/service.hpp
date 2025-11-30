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
#include "srp/apps/RecoveryServiceSkeleton.h"
#include "apps/fc/recovery_service/parachute_controller.hpp"

namespace srp {
namespace apps {
class MyRecoveryServiceSkeleton: public RecoveryServiceSkeleton {
 private:
  const std::shared_ptr<recovery::ParachuteController> controller;

 public:
  MyRecoveryServiceSkeleton(const ara::core::InstanceSpecifier& instance,
                      const std::shared_ptr<recovery::ParachuteController>& contr):
                  RecoveryServiceSkeleton{instance}, controller(contr) {}
  virtual ~MyRecoveryServiceSkeleton() = default;

  srp::apps::RecoveryService::NewParachuteStatusEventEventSkeleton NewParachuteStatusEvent{};

 protected:
  ara::core::Result<bool> OpenReefedParachute() override {
    return controller->OpenParachute();
  }
  ara::core::Result<bool> UnreefeParachute() override {
    return controller->UnreefParachute();
  }
};
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RECOVERY_SERVICE_SERVICE_HPP_
