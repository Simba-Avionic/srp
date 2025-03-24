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
#include "core/common/condition.h"
#include "ara/log/log.h"
namespace srp {
namespace apps {
namespace recovery {

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

void ParachuteController::Init(std::unique_ptr<i2c::PCA9685>&& servo,
                              std::unique_ptr<gpio::GPIOController>&& gpio, const std::string& path) {
    auto config_opt = read_config(core::json::JsonParser::Parser(path + "etc/config.json"));
    this->config_ = config_opt.value();
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
    this->gpio_controller->SetPinValue(config_.servo_mosfet_id, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(config_.mosfet_delay));
    for (uint8_t i = 0; i < config_.Servo_sequence_num; i++) {
      this->servo_controller->AutoSetServoPosition(config_.Recovery_servo_id, 1);
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.Servo_move_time));
      this->servo_controller->AutoSetServoPosition(config_.Recovery_servo_id, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.Servo_move_time));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(config_.mosfet_delay));
    this->gpio_controller->SetPinValue(config_.servo_mosfet_id, 0);
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
      this->gpio_controller->SetPinValue(config_.linecutter_pin_id, 1);
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.Linecutter_active_time));
      this->gpio_controller->SetPinValue(config_.linecutter_pin_id, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.Linecutter_inactive_time));
    }
    return true;
}
ParachuteController::ParachuteController(): parachute_unreefed(false), parachute_open(false) {
}


}  // namespace recovery
}  // namespace apps
}  // namespace srp
