/**
 * @file servo_controller.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "apps/ec/ServoService/servoController/servo_controller.hpp"

#include <chrono>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <utility>
#include <algorithm>

#include "core/common/condition.h"

namespace srp {
namespace service {

ServoController::ServoController():
      logger_(ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "srct", "", ara::log::LogLevel::kDebug)) {}

void ServoController::closingThreadLoop(const std::stop_token& token) {
    while (!token.stop_requested()) {
      auto now = Clock::now();
      auto can_sleep_for = std::chrono::milliseconds(1000);
      const auto ids = servo_cfg_mng.GetServosID();
      for (const auto& id : ids) {
        const auto cfg = servo_cfg_mng.GetServoConfig(id);
        if (!cfg.has_value()) {
          continue;
        }
        if (cfg.value().auto_closing == 0) {
          continue;
        }
        if (cfg.value().position != kOpenState) {
          continue;
        }
        if (cfg.value().open_time_end <= now) {
          servo_ctr_.SetServoPosition(cfg.value(), 0);
        } else {
          auto time_until_end = std::chrono::duration_cast<
                  std::chrono::milliseconds>(cfg.value().open_time_end - now);
          can_sleep_for = std::min(can_sleep_for, time_until_end);
          can_sleep_for = std::max(can_sleep_for, std::chrono::milliseconds(1));
        }
      }
      core::condition::wait_for(std::chrono::milliseconds(can_sleep_for), token);
    }
}

void ServoController::pulsingThreadLoop(const std::stop_token& token) {
  while (!token.stop_requested()) {
    auto now = Clock::now();
    auto can_sleep_for = std::chrono::milliseconds(1000);
    const auto ids = servo_cfg_mng.GetServosID();
    for (const auto& id : ids) {
      auto cfg = servo_cfg_mng.GetServoConfig(id);
      if (!cfg.has_value()) {
        continue;
      }
      if (cfg.value().pulsing_time == 0) {
        continue;
      }
      if (cfg.value().position == kCloseState) {
        pulsing_db.erase(id);
        servo_ctr_.SetServoPosition(cfg.value(), kCloseState);
        continue;
      }
      {
        std::lock_guard<std::mutex> lock(pulsing_mtx_);
        auto pulse_cfg = pulsing_db.find(id);
        if (pulse_cfg == pulsing_db.end()) {
          continue;
        }
        auto now = Clock::now();
        if (now >= pulse_cfg->second.pulse_deadline) {
          pulse_cfg->second.pulse_state = !pulse_cfg->second.pulse_state;
          servo_ctr_.SetServoPosition(cfg.value(), pulse_cfg->second.pulse_state);
          pulse_cfg->second.pulse_deadline = now + std::chrono::milliseconds(cfg.value().pulsing_time);
        }
        auto new_sleep_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(pulse_cfg->second.pulse_deadline - now);
        can_sleep_for = std::min(can_sleep_for, new_sleep_time);
        can_sleep_for = std::max(can_sleep_for, std::chrono::milliseconds(1));
      }
    }
    core::condition::wait_for(can_sleep_for, token);
  }
}


void ServoController::Init(const std::string& app_path) {
  logger_.LogInfo() << "ServoController.Init: start initialization for path " << app_path;

  servo_cfg_mng.LoadServoConfig(app_path + "etc/config.json");

  servo_ctr_.Init();

  closing_thread = std::jthread([this](const std::stop_token& t) { closingThreadLoop(t); });
  pulsing_thread = std::jthread([this](const std::stop_token& t) { pulsingThreadLoop(t); });

  logger_.LogInfo() << "ServoController.Init: initialization completed";
  auto servo_ids = servo_cfg_mng.GetServosID();
  for (const auto& servo : servo_ids) {
    AutoSetServoPosition(servo, 0, true);
  }
  logger_.LogInfo() << "ServoController.Init: default servos position setted";
}

bool ServoController::AutoSetServoPosition(const uint8_t actuator_id,
                                                      const uint8_t state, bool force) {
  const auto cfg = servo_cfg_mng.GetServoConfig(actuator_id);
  if (!cfg.has_value()) {
    logger_.LogWarn() << "ServoController.AutoSetServoPosition: unknown actuator_id "
                      << actuator_id;
    return false;
  }
  auto& servo = cfg.value();
  logger_.LogInfo() << "ServoController.ExecuteServoMovement: actuator "
                    << actuator_id << " channel " << servo.channel
                    << " target state " << state;

  logger_.LogInfo() << "ServoController.ExecuteServoMovement: actuator "
                    << actuator_id << " moved successfully, state " << state;

  if (!force) {
    if (cfg.value().position == state) {
      return true;
    }
  }

  if (!servo_ctr_.SetServoPosition(servo, state)) {
    logger_.LogError() << "ServoController.AutoSetServoPosition: failed to move actuator "
                       << actuator_id << " to state " << state;
    return false;
  }
  servo_cfg_mng.SetServoPosition(actuator_id, state);

  logger_.LogInfo() << "ServoController.AutoSetServoPosition: actuator "
                    << actuator_id << " moved to state " << state;

  if (state == 2) {
    return true;
  }
  if (cfg.value().pulsing_time != 0) {
    const auto pulse_deadline = Clock::now() + std::chrono::milliseconds(cfg.value().pulsing_time);
    std::lock_guard<std::mutex> lock(pulsing_mtx_);
    pulsing_db[actuator_id] = Pulsing_t{.pulse_state = 1, .pulse_deadline = pulse_deadline};
    logger_.LogDebug() << "ServoController.AutoSetServoPosition: enabled pulsing for actuator "
                       << actuator_id << ", interval_ms " << cfg.value().pulsing_time;
  }


  return true;
}

std::optional<uint8_t> ServoController::ReadServoPosition(const uint8_t actuator_id) {
  auto cfg = servo_cfg_mng.GetServoConfig(actuator_id);
  if (!cfg.has_value()) {
    logger_.LogWarn() << "ServoController.ReadServoPosition: can't find actuator with id "
                      << actuator_id;
    return std::nullopt;
  }
  return cfg.value().position;
}

bool ServoController::ChangeConfigPosition(const uint8_t actuator_id,
                                           const uint16_t new_open_val,
                                           const uint16_t new_close_val) {
  if (!servo_cfg_mng.ChangeServoConfigPosition(actuator_id, new_open_val, new_close_val)) {
    logger_.LogWarn() << "ServoController.ChangeConfigPosition: failed to update config for actuator "
                      << actuator_id;
    return false;
  }
  auto cfg = servo_cfg_mng.GetServoConfig(actuator_id);
  if (!cfg.has_value()) {
    logger_.LogError() << "ServoController.ChangeConfigPosition: config disappeared for actuator "
                       << actuator_id;
    return false;
  }
  const auto res = servo_ctr_.SetServoPosition(cfg.value(), kCloseState);
  if (!res) {
    logger_.LogError() << "ServoController.ChangeConfigPosition: failed to apply close state for actuator "
                       << actuator_id;
    return false;
  }
  logger_.LogInfo() << "ServoController.ChangeConfigPosition: updated actuator "
                    << actuator_id << " open " << new_open_val
                    << " close " << new_close_val;
  return true;
}


}  // namespace service
}  // namespace srp


