/**
 * @file gs_hb_guider.cpp
 * @author Mateusz Krajewski (Mateusz Krajewski)
 * @brief 
 * @version 0.1
 * @date 2026-04-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "apps/fc/radio_service/gs_hb_guider.hpp"
#include <chrono>  // NOLINT
#include <thread>  // NOLINT
#include <string>
#include "ara/log/log.h"
#include "core/common/condition.h"
namespace srp {
namespace apps {
namespace radio {
namespace {
  static constexpr auto kHbDelayWarning = 1.1;
  static constexpr auto kDuration_from_last_hb_to_conn_lost_ms = 2 * 60 * 1000;
  static constexpr auto kDuration_from_last_hb_to_abort_ms = 15 * 60 * 1000;
}

void GSHeartbeatGuard::OnNewHbCallback() {
  auto now = std::chrono::high_resolution_clock::now();
  {
    std::lock_guard lock(hb_guard_mtx_);
    if (!first_hb_received_) {
      first_hb_received_ = true;
      last_hb_time_ = now;
      return;
    }
    last_hb_time_ = now;
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_hb_time_).count();
  if (duration > (1000 * kHbDelayWarning)) {
    ara::log::LogWarn() << "GS Heartbeat received with duration: " << std::to_string(duration);
  } else {
    ara::log::LogDebug() << "GS Heartbeat received with duration: " << std::to_string(duration);
  }
}

GSHeartbeatGuard::GSHeartbeatGuard(): first_hb_received_(false) {
}

void GSHeartbeatGuard::Init(GsConLossCB cb) {
  this->callback = cb;
  guard_thread = std::jthread([this](const std::stop_token& t){
    GSHeartbeatGuardLoop(t);
  });
}

void GSHeartbeatGuard::GSHeartbeatGuardLoop(const std::stop_token& t) {
  auto broadcast_state = [this](RocketState_t state, const std::string& reason) {
      ara::log::LogError() << "GS Connection Guard: " << reason << " -> Switching to "
                            << core::rocketState::to_string(state);
      this->callback(state);
  };
  bool first_hb_received;
  timepoint last_hb_time;
  while (!t.stop_requested()) {
    core::condition::wait_for(std::chrono::milliseconds(1000), t);
    {
      std::lock_guard<std::mutex> lock(hb_guard_mtx_);
      first_hb_received = first_hb_received_;
      last_hb_time = last_hb_time_;
    }
    if (!first_hb_received) {
      continue;
    }
    auto now = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_hb_time).count();

    if (diff >= kDuration_from_last_hb_to_abort_ms) {
        broadcast_state(RocketState_t::ABORT, "Timeout ABORT");
    } else if (diff >= kDuration_from_last_hb_to_conn_lost_ms) {
        broadcast_state(RocketState_t::CONNECTION_LOST, "Timeout CONN_LOST");
    }
  }
}

}  // namespace radio
}  // namespace apps
}  // namespace srp
