/**
 * @file parachute_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/fc/recovery_service/parachute_controller.hpp"
#include <chrono>  // NOLINT
#include <cstdint>
#include <stdexcept>
#include <thread>  // NOLINT
#include <utility>
#include "core/json/json_parser.h"
#include "core/common/condition.h"
#include "ara/log/log.h"
namespace srp {
namespace apps {
namespace recovery {

namespace {
  std::shared_ptr<ParachuteController> instance = nullptr;
}

std::shared_ptr<ParachuteController> ParachuteController::GetInstance() {
  if (instance == nullptr) {
    instance = std::make_shared<ParachuteController>();
  }
  return instance;
}


std::optional<Parachute_config_t> ParachuteController::read_config(std::optional<srp::core::json::JsonParser> parser_) {
  Parachute_config_t config;
  if (!parser_.has_value()) {
    return std::nullopt;
  }
  auto obj_opt = parser_.value().GetObject("parachute");
  if (!obj_opt.has_value()) {
    return std::nullopt;
  }
  auto obj = obj_opt.value();
  auto get_val = [&obj](const std::string& key) {
    auto val = obj.GetNumber<uint16_t>(key);
    if (!val.has_value()) {
      ara::log::LogError() << "Value with name: " << key << ", not define";
      exit(1);
    }
    return obj.GetNumber<uint16_t>(key).value();
  };
  config.servo_mosfet_id = static_cast<uint8_t>(get_val("mosfet_id"));
  config.mosfet_delay = get_val("mosfet_delay");
  config.linecutter_pin_id = static_cast<uint8_t>(get_val("linecutter_pin_id"));
  config.Recovery_servo_id = static_cast<uint8_t>(get_val("recovery_servo_id"));
  config.Servo_move_time = get_val("servo_move_time");
  config.Servo_sequence_num = static_cast<uint8_t>(get_val("servo_seq_num"));
  config.Linecutter_sequence_num = static_cast<uint8_t>(get_val("linecutter_sequence_num"));
  config.Linecutter_active_time = get_val("linecutter_active_time");
  config.Linecutter_inactive_time = get_val("linecutter_inactive_time");
  config.backup_linecutter_activation_time = get_val("backup_linecutter_activation_time");
  config.linecutter_active_height = get_val("linecutter_active_height");
  return config;
}

void ParachuteController::Init(const std::string& path) {
  auto config_opt = read_config(core::json::JsonParser::Parser(path + "etc/config.json"));
  if (!config_opt.has_value()) {
    ara::log::LogError() << "ParachuteController.Init: invalid config at " << path;
    throw std::runtime_error("Missing parachute config");
  }
  std::lock_guard<std::mutex> lock(config_mtx_);
  config_ = config_opt.value();
  servo_controller.Init(path);
}

void ParachuteController::JoinFinishedWorkerIfNeeded() {
  if (operation_thread_.joinable() && !operation_in_progress_.load()) {
    operation_thread_.join();
  }
}

bool ParachuteController::OpenParachute(const std::stop_token& token) {
  if (parachute_state.load() != ParachuteState_t::CLOSED) {
    return false;
  }

  if (operation_in_progress_) {
    return false;
  }

  std::lock_guard<std::mutex> guard(operation_mtx_);
  JoinFinishedWorkerIfNeeded();

  parachute_state.store(ParachuteState_t::OPENING_REEFED);

  operation_thread_ = std::jthread([this, token]() {
    bool cancelled = false;
    Parachute_config_t cfg;
    {
      std::lock_guard<std::mutex> lock(config_mtx_);
      cfg = config_;
    }

    uint32_t active_time = static_cast<uint32_t>(cfg.mosfet_delay) * 2U +
                           static_cast<uint32_t>(cfg.Servo_move_time) *
                               static_cast<uint32_t>(cfg.Servo_sequence_num) * 2U;
    gpio_controller.SetPinValue(cfg.servo_mosfet_id, 1, static_cast<uint16_t>(active_time));
    core::condition::wait_for(std::chrono::milliseconds(active_time), token);
    if (token.stop_requested()) {
      cancelled = true;
    }

    for (uint8_t i = 0; i < cfg.Servo_sequence_num && !cancelled; ++i) {
      servo_controller.AutoSetServoPosition(cfg.Recovery_servo_id, 1);
      core::condition::wait_for(std::chrono::milliseconds(cfg.Servo_move_time), token);
      if (token.stop_requested()) {
      cancelled = true;
      break;
      }

      servo_controller.AutoSetServoPosition(cfg.Recovery_servo_id, 0);
      core::condition::wait_for(std::chrono::milliseconds(cfg.Servo_move_time), token);
      if (token.stop_requested()) {
        cancelled = true;
        break;
      }
    }
    parachute_state.store(cancelled ? ParachuteState_t::ERROR
                                      : ParachuteState_t::OPEN_REEFED);
    operation_in_progress_.store(false);
  });
  return true;
}

bool ParachuteController::UnreefParachute(const std::stop_token& token) {
  if (parachute_state.load() != ParachuteState_t::OPEN_REEFED) {
    return false;
  }

  if (operation_in_progress_) {
    return false;
  }

  std::lock_guard<std::mutex> guard(operation_mtx_);
  JoinFinishedWorkerIfNeeded();

  parachute_state.store(ParachuteState_t::OPENING_UNREEFED);

  operation_thread_ = std::jthread([this, token]() {
    bool cancelled = false;
    Parachute_config_t cfg;
    {
      std::lock_guard<std::mutex> lock(config_mtx_);
      cfg = config_;
    }

    auto sleep_time_ms = std::chrono::milliseconds(cfg.Linecutter_active_time + cfg.Linecutter_inactive_time);

    for (uint8_t i = 0; i < cfg.Linecutter_sequence_num; ++i) {
      gpio_controller.SetPinValue(cfg.linecutter_pin_id, 1, cfg.Linecutter_active_time);
      core::condition::wait_for(sleep_time_ms, token);
      if (token.stop_requested()) {
        cancelled = true;
        break;
      }
    }

    parachute_state.store(cancelled ? ParachuteState_t::ERROR
                                      : ParachuteState_t::OPEN_UNREEFED);
    operation_in_progress_.store(false);
  });
  return true;
}

void ParachuteController::StopThred() {
  std::lock_guard<std::mutex> guard(operation_mtx_);
  if (operation_thread_.joinable()) {
    operation_thread_.request_stop();
    operation_thread_.join();
  }
  operation_in_progress_.store(false);
}


ParachuteController::ParachuteController()
    : parachute_state(ParachuteState_t::CLOSED),
      operation_in_progress_(false) {
}


}  // namespace recovery
}  // namespace apps
}  // namespace srp
