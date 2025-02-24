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
#include <utility>
#include "core/json/json_parser.h"
namespace srp {
namespace apps {
namespace recovery {

namespace {
  constexpr auto kDefLinecutter_pin_id = 1;
  constexpr auto kDefRecovery_servo_id = 63;
  constexpr auto kDefServo_move_time = 500;
  constexpr auto kDefServo_sequence_num = 3;
  constexpr auto kDefLinecutter_sequence_num = 3;
  constexpr auto kDefLinecutter_active_time = 3000;
  constexpr auto kDefLinecutter_inactive_time = 1000;
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
  auto get_or_default = [&obj](const std::string& key, uint16_t default_value) {
    return obj.GetNumber<decltype(default_value)>(key).value_or(default_value);
  };
  config.Linecutter_pin_id = get_or_default("linecutter_pin_id", kDefLinecutter_pin_id);
  config.Recovery_servo_id = get_or_default("recovery_servo_id", kDefRecovery_servo_id);
  config.Servo_move_time = get_or_default("servo_move_time", kDefServo_move_time);
  config.Servo_sequence_num = get_or_default("servo_seq_num", kDefServo_sequence_num);
  config.Linecutter_sequence_num = get_or_default("linecutter_sequence_num", kDefLinecutter_sequence_num);
  config.Linecutter_active_time = get_or_default("linecutter_active_time", kDefLinecutter_active_time);
  config.Linecutter_inactive_time = get_or_default("linecutter_inactive_time", kDefLinecutter_inactive_time);
  return config;
}

void ParachuteController::Init(std::unique_ptr<i2c::PCA9685>&& servo,
                              std::unique_ptr<gpio::GPIOController>&& gpio, const std::string& path) {
    auto config_opt = read_config(core::json::JsonParser::Parser(path + "etc/config.json"));
    this->config_ = config_opt.value_or(Parachute_config_t{
      kDefLinecutter_pin_id,
      kDefRecovery_servo_id,
      kDefServo_move_time,
      kDefServo_sequence_num,
      kDefLinecutter_sequence_num,
      kDefLinecutter_active_time,
      kDefLinecutter_inactive_time,
    });
    this->servo_controller = std::move(servo);
    this->gpio_controller = std::move(gpio);
}

bool ParachuteController::OpenParachute(bool diag) {
    if (!servo_controller) {
        return false;
    }
    if (parachute_open && !diag) {
      return false;
    }
    if (!diag) {
      parachute_open = true;
    }
    for (uint8_t i = 0; i < config_.Servo_sequence_num; i++) {
      this->servo_controller->AutoSetServoPosition(config_.Recovery_servo_id, 1);
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.Servo_move_time));
      this->servo_controller->AutoSetServoPosition(config_.Recovery_servo_id, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.Servo_move_time));
    }
    return true;
}
bool ParachuteController::UnreefParachute(bool diag) {
    if (!gpio_controller) {
        return false;
    }
    if ((parachute_unreefed || !parachute_open) && !diag) {
      return true;
    }
    if (!diag) {
      parachute_unreefed = true;
    }
    for (auto i = 0; i < config_.Linecutter_sequence_num; i++) {
      this->gpio_controller->SetPinValue(config_.Linecutter_pin_id, 1);
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.Linecutter_active_time));
      this->gpio_controller->SetPinValue(config_.Linecutter_pin_id, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.Linecutter_inactive_time));
    }
    return true;
}
ParachuteController::ParachuteController(): parachute_unreefed(false), parachute_open(false) {
}


}  // namespace recovery
}  // namespace apps
}  // namespace srp
