/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_FC_MAIN_SERVICE_SERVICE_HPP_
#define APPS_FC_MAIN_SERVICE_SERVICE_HPP_
#include <memory>
#include <unordered_map>
#include <vector>
#include "srp/apps/MainServiceSkeleton.h"
#include "core/rocket_machine_state/rocket_state.hpp"
#include "ara/log/log.h"
namespace srp {
namespace apps {

class MyMainServiceSkeleton: public MainServiceSkeleton {
 private:
  using RocketState_t = core::rocketState::RocketState_t;

  std::shared_ptr<core::rocketState::RocketStateController> state_ctr;

 public:
  explicit MyMainServiceSkeleton(const ara::core::InstanceSpecifier& instance):
                MainServiceSkeleton{instance} {
    state_ctr = core::rocketState::RocketStateController::GetInstance();
  }
  virtual ~MyMainServiceSkeleton() = default;

 protected:
  ara::core::Result<bool> setMode(const std::uint8_t& in_parm) override {
    return state_ctr->SetState(static_cast<RocketState_t>(in_parm));
  }
};
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_MAIN_SERVICE_SERVICE_HPP_
