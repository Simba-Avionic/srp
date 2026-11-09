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
#include <optional>
#include "ara/log/log.h"
#include "core/common/condition.h"
namespace srp {
namespace apps {
namespace radio {
namespace {
  static constexpr auto kHbDelayWarning = 1.1;
  static constexpr auto kDuration_from_last_hb_to_conn_lost_ms = 5 * 60 * 1000;
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
    ara::log::LogWarn() << "GS Heartbeat received with duration: " << duration;
  } else {
    ara::log::LogDebug() << "GS Heartbeat received with duration: " << duration;
  }
}

GSHeartbeatGuard::GSHeartbeatGuard(): first_hb_received_(false) {
}

bool GSHeartbeatGuard::IsFlightPhase(RocketState_t state) {
  // the states 3..9 (LAUNCH ... TOUCHDOWN); CONNECTION_LOST (54) and ABORT (64) are outside of this range
  return state >= RocketState_t::LAUNCH && state <= RocketState_t::TOUCHDOWN;
}

std::optional<RocketState_t> GSHeartbeatGuard::Evaluate(RocketState_t current, int64_t ms_since_last_hb) {
  if (IsFlightPhase(current)) {
    return std::nullopt;  // a lost connection during the flight does nothing
  }
  if (ms_since_last_hb >= kDuration_from_last_hb_to_abort_ms) {
    return RocketState_t::ABORT;
  }
  if (ms_since_last_hb >= kDuration_from_last_hb_to_conn_lost_ms) {
    return RocketState_t::CONNECTION_LOST;
  }
  return std::nullopt;
}

void GSHeartbeatGuard::Init(GsConLossCB cb, RocketStateProviderCB provider) {
  this->callback = cb;
  this->state_provider = provider;
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
  bool flight_loss_logged = false;
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

    const auto current = state_provider ? state_provider() : RocketState_t::INIT;
    if (IsFlightPhase(current)) {
      // from LAUNCH on the mechanism is switched off: nothing is done, only one Info per episode of the lost link
      if (diff >= kDuration_from_last_hb_to_conn_lost_ms && !flight_loss_logged) {
        flight_loss_logged = true;
        ara::log::LogInfo() << "GS Connection Guard: GS heartbeat lost for " << diff << " ms in state "
                            << core::rocketState::to_string(current)
                            << ", ignored (the guard works only before LAUNCH)";
      } else if (diff < kDuration_from_last_hb_to_conn_lost_ms) {
        flight_loss_logged = false;
      }
      continue;
    }
    flight_loss_logged = false;
    const auto target = Evaluate(current, diff);
    if (target.has_value()) {
      broadcast_state(target.value(), target.value() == RocketState_t::ABORT ? "Timeout ABORT" : "Timeout CONN_LOST");
    }
  }
}

}  // namespace radio
}  // namespace apps
}  // namespace srp
