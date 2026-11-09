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
#ifndef APPS_EC_RADIO_SERVICE_GS_HB_GUIDER_HPP_
#define APPS_EC_RADIO_SERVICE_GS_HB_GUIDER_HPP_
#include <chrono>  // NOLINT
#include <cstdint>
#include <functional>
#include <stop_token>  // NOLINT
#include <thread>  // NOLINT
#include <mutex>  // NOLINT
#include <optional>
#include "core/rocket_machine_state/rocket_state.hpp"
namespace srp {
namespace apps {
namespace radio {
using timepoint = std::chrono::_V2::high_resolution_clock::time_point;
using RocketState_t = core::rocketState::RocketState_t;
using GsConLossCB = std::function<void(const RocketState_t)>;
// state of the rocket seen by the radio (the guard must not act from LAUNCH on)
using RocketStateProviderCB = std::function<RocketState_t()>;
class GSHeartbeatGuard {
 private:
  std::mutex hb_guard_mtx_;
  timepoint last_hb_time_;
  bool first_hb_received_;

  GsConLossCB callback;
  RocketStateProviderCB state_provider;
  std::jthread guard_thread;
  void GSHeartbeatGuardLoop(const std::stop_token& t);

 public:
  GSHeartbeatGuard();
  void OnNewHbCallback();
  /// @brief starts the watchdog thread
  /// @param cb called with the state to switch to when the GS heartbeat is lost
  /// @param state_provider current state of the rocket; the guard reacts to a lost connection only BEFORE LAUNCH
  void Init(GsConLossCB cb, RocketStateProviderCB state_provider);
  /// @return true for the states from LAUNCH on (LAUNCH, FLIGHT, APOGEE ... TOUCHDOWN): the guard is switched off
  static bool IsFlightPhase(RocketState_t state);
  /// @brief decision of the watchdog
  /// @param current current state of the rocket
  /// @param ms_since_last_hb time since the last GS heartbeat
  /// @return the state to switch to or nullopt (no action: connection fine, or flight phase)
  static std::optional<RocketState_t> Evaluate(RocketState_t current, int64_t ms_since_last_hb);
};

}  // namespace radio
}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_RADIO_SERVICE_GS_HB_GUIDER_HPP_
