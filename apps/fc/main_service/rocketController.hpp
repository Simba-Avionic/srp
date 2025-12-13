/**
 * @file rocketController.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_FC_MAIN_SERVICE_ROCKETCONTROLLER_HPP_
#define APPS_FC_MAIN_SERVICE_ROCKETCONTROLLER_HPP_
#include <memory>
#include "apps/fc/main_service/rocket_state.h"
#include "core/common/error_code.h"
#include "core/timestamp/timestamp_driver.hpp"
namespace srp {
namespace apps {

class RocketController {
 private:
  std::shared_ptr<RocketState> rocket_state_;
  apps::RocketState_t last_state_;
  srp::core::timestamp::TimestampController timestamp_;
  core::ErrorCode InitializeCompleted();
  core::ErrorCode ActivateTankActuators();
  core::ErrorCode ArmRocket();
  core::ErrorCode CutdownEndSeq();
  core::ErrorCode LossConnSeq();

 public:
  void Loop();
  void Init();
  RocketController();
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_MAIN_SERVICE_ROCKETCONTROLLER_HPP_
