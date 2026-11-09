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
#ifndef APPS_FC_RADIO_SERVICE_GS_HB_GUIDER_HPP_
#define APPS_FC_RADIO_SERVICE_GS_HB_GUIDER_HPP_
#include <atomic>
#include <chrono>  // NOLINT
#include <cstdint>
#include <functional>
#include <stop_token>  // NOLINT
#include <thread>  // NOLINT
#include <mutex>  // NOLINT
#include "core/rocket_machine_state/rocket_state.hpp"
namespace srp {
namespace apps {
namespace radio {
using timepoint = std::chrono::_V2::high_resolution_clock::time_point;
using RocketState_t = core::rocketState::RocketState_t;
using GsConLossCB = std::function<void(const RocketState_t)>;
class GSHeartbeatGuard {
 private:
  std::mutex hb_guard_mtx_;
  timepoint last_hb_time_;
  bool first_hb_received_;
  std::atomic<int64_t> last_interval_ms_{0};

  GsConLossCB callback;
  std::jthread guard_thread;
  void GSHeartbeatGuardLoop(const std::stop_token& t);

 public:
  GSHeartbeatGuard();
  void OnNewHbCallback();
  /// Odstep miedzy dwoma ostatnimi heartbeatami [ms] (0 przed drugim heartbeatem).
  int64_t LastIntervalMs() const noexcept { return last_interval_ms_.load(); }
  void Init(GsConLossCB cb);
};

}  // namespace radio
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_GS_HB_GUIDER_HPP_
