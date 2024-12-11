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

#include "simba/apps/RecoveryServiceSkeleton.h"

namespace simba {
namespace apps {
class MyRecoveryServiceSkeleton: public RecoveryServiceSkeleton {
 public:
  explicit MyRecoveryServiceSkeleton(const ara::core::InstanceSpecifier& instance): RecoveryServiceSkeleton{instance} {}
  virtual ~MyRecoveryServiceSkeleton() = default;

  simba::apps::RecoveryService::NewParachuteStatusEventEventSkeleton NewParachuteStatusEvent{};
 protected:
  ara::core::Result<bool> OpenReefedParachute() override {
    return ara::com::MakeErrorCode(ara::com::ComErrc::kSetHandlerNotSet, "Method handler is not set");
  }
  ara::core::Result<bool> UnreefeParachute() override {
    return ara::com::MakeErrorCode(ara::com::ComErrc::kSetHandlerNotSet, "Method handler is not set");
  }
};
}  // namespace apps
}  // namespace simba

#endif  // APPS_FC_RECOVERY_SERVICE_SERVICE_HPP_
