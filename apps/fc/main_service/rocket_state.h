/**
 * @file rocket_state.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_FC_MAIN_SERVICE_ROCKET_STATE_H_
#define APPS_FC_MAIN_SERVICE_ROCKET_STATE_H_
#include <mutex>  // NOLINT
#include <shared_mutex>
#include <memory>
#include <utility>
#include <string>
#include "simba/mavlink.h"
namespace srp {
namespace apps {
class RocketState;
static std::shared_ptr<RocketState> rocket_state = nullptr;

enum RocketState_t {
  INIT = 0,
  DISARM = SIMBA_ROCKET_STATE_DISARM,
  ARM = SIMBA_ROCKET_STATE_ARM,
  LAUNCH = SIMBA_ROCKET_STATE_LAUNCH,
  FLIGHT = SIMBA_ROCKET_STATE_FLY,
  MAIN_PARACHUTE_TRIGGERED = SIMBA_ROCKET_STATE_MAIN_PARACHUTE_TRIGGERED,
  SECOND_PARACHUTE_TRIGGERED = SIMBA_ROCKET_STATE_SECOND_PARACHUTE_TRIGGERED,
  ABORT = SIMBA_ROCKET_STATE_ABORT,
};

class RocketState {
 private:
  RocketState_t state_;
  std::shared_mutex mtx_;

 public:
  static std::shared_ptr<RocketState> GetInstance() {
    if (rocket_state == nullptr) {
        rocket_state = std::make_shared<RocketState>();
    }
    return rocket_state;
  }
  RocketState_t GetState() {
    std::shared_lock lock(mtx_);
    return this->state_;
  }
  static std::string GetStringState(const RocketState_t state) {
    std::string res;
    switch (state) {
    case INIT:
      res = "INIT";
      break;
    case DISARM:
      res = "DISARM";
      break;
    case ARM:
      res = "ARM";
      break;
    case LAUNCH:
      res = "LAUNCH";
      break;
    case FLIGHT:
      res = "FLIGHT";
      break;
    case MAIN_PARACHUTE_TRIGGERED:
      res = "MAIN_PARACHUTE_TRIGGERED";
      break;
    case SECOND_PARACHUTE_TRIGGERED:
      res = "SECOND_PARACHUTE_TRIGGERED";
      break;
    case ABORT:
      res = "ABORT";
      break;
    default:
      break;
    }
    return res;
  }
  void SetState(const RocketState_t state) {
    std::unique_lock lock(mtx_);
    this->state_ = state;
  }
};


}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_MAIN_SERVICE_ROCKET_STATE_H_
