/**
 * @file state_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-12-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_EC_ENGINE_SERVICE_STATE_CONTROLLER_HPP_
#define APPS_EC_ENGINE_SERVICE_STATE_CONTROLLER_HPP_
#include <mutex>  // NOLINT
#include <memory>
#include "apps/fc/main_service/rocket_state.h"
namespace srp {
namespace engineApp {
class EngineStateController {
 private:
  apps::RocketState_t state_;
  std::mutex state_mtx_;
 public:
  EngineStateController();
  static std::shared_ptr<EngineStateController> GetInstance();
  apps::RocketState_t GetEngineState();
  void SetEngineState(const apps::RocketState_t state);
};
}  // namespace engineApp
}  // namespace srp

#endif  // APPS_EC_ENGINE_SERVICE_STATE_CONTROLLER_HPP_
