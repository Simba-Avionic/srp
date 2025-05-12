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
namespace srp {
namespace apps {
class RocketState;
static std::shared_ptr<RocketState> rocket_state = nullptr;

enum RocketState_t {
  INIT = 0,
  DISARM = 1,
  TANK = 2,
  ARM = 3,
  CUTDOWN = 4,
  CUTDOWN_END = 5,
  FLIGHT = 6,
  FALL = 7,
  LANDED = 8,
  ABORD = 50,
  LOST_CONN = 52
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
  void SetState(const RocketState_t state) {
    std::unique_lock lock(mtx_);
    this->state_ = state;
  }
};


}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_MAIN_SERVICE_ROCKET_STATE_H_
