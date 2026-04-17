/**
 * @file gs_hb_guider.hpp
 * @author Mateusz Krajewski (Mateusz Krajewski)
 * @brief 
 * @version 0.1
 * @date 2026-04-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef x
#define x
#include <chrono>  // NOLINT
#include <functional>
#include <stop_token>
#include <thread>  // NOLINT
#include "core/rocket_machine_state/rocket_state.hpp"
namespace srp {
namespace apps {
namespace radio {
using timepoint = std::chrono::_V2::high_resolution_clock::time_point;
using RocketState_t = core::rocketState::RocketState_t;
using GsConLossCB = std::function<void(const RocketState_t)>;
class GSHeartbeatGuard {
 private:
  timepoint last_hb_time;
  bool first_hb_received;
  GsConLossCB callback;
  std::jthread guard_thread;
 public:
  GSHeartbeatGuard();
  void OnNewHbCallback();
  void Init(GsConLossCB cb);
  void GSHeartbeatGuardLoop(const std::stop_token& t);
};

}  // namespace radio
}  // namespace apps
}  // namespace srp

#endif  // x
