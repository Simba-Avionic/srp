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
#include <stop_token>
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
    std::stop_source never_stop_source;
    auto res = controller->OpenParachute(never_stop_source.get_token(), false);
    if (!res) {
      return false;
    }
    this->NewParachuteStatusEvent.Update(static_cast<uint8_t>(recovery::ParachuteState_t::OPEN_REEFED));
    return res;
  }
  ara::core::Result<bool> UnreefeParachute() override {
    std::stop_source never_stop_source;
    auto res = controller->UnreefParachute(never_stop_source.get_token(), false);
    if (!res) {
      return false;
    }
    this->NewParachuteStatusEvent.Update(static_cast<uint8_t>(recovery::ParachuteState_t::OPEN_UNREEFED));
    return res;
  }
};
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RECOVERY_SERVICE_SERVICE_HPP_
