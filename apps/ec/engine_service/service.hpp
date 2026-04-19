/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef APPS_EC_ENGINE_SERVICE_SERVICE_HPP_
#define APPS_EC_ENGINE_SERVICE_SERVICE_HPP_

#include <memory>

#include "srp/apps/EngineServiceSkeleton.h"
#include "core/rocket_machine_state/rocket_state.hpp"
#include "apps/ec/engine_service/vent_controller.hpp"

namespace srp {
namespace apps {
namespace service {


class MyEngineServiceSkeleton: public EngineServiceSkeleton {
 private:
  std::shared_ptr<core::rocketState::RocketStateController> state_ctr_;
  std::shared_ptr<engine::VentController> vent_ctr_;
 public:
  explicit MyEngineServiceSkeleton(ara::core::InstanceSpecifier instance);
 protected:
  ara::core::Result<bool> SetMode(const std::uint8_t& in_parm) override;
  ara::core::Result<std::uint8_t> GetMode() override;
  ara::core::Result<void> SetVentValve(const std::uint8_t& in_parm) override;
};

}  // namespace service
}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_ENGINE_SERVICE_SERVICE_HPP_
