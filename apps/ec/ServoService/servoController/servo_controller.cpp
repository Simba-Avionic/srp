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

namespace srp {
namespace service {

namespace {
constexpr uint8_t kOpenState = 1U;
constexpr uint8_t kCloseState = 0U;
constexpr uint16_t kDefaultMosfetPowerOnDelayMs = 50U;
constexpr uint16_t kDefaultServoMoveTimeMs = 400U;
constexpr uint16_t kDefaultLooseningDelayMs = 50U;
}  // namespace

ServoController::ServoController()
    : driver_(nullptr),
      gpio_(nullptr),
      logger_(ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "srvServo", "", ara::log::LogLevel::kDebug)) {}

core::ErrorCode ServoController::Init(
    const std::string& app_path, std::shared_ptr<srp::i2c::PCA9685> driver,
    std::unique_ptr<gpio::IGPIOController> gpio,
    std::unique_ptr<srp::i2c::II2CController> i2c_impl) {
  logger_.LogInfo() << "ServoController.Init: start initialization for path " << app_path;
  driver_ = std::move(driver);
  gpio_ = std::move(gpio);
  if (!driver_) {
    logger_.LogError() << "ServoController.Init: PCA9685 driver is null";
    return core::ErrorCode::kInitializeError;
  }
  if (driver_->Init(std::move(i2c_impl)) != core::ErrorCode::kOk) {
    logger_.LogError() << "ServoController.Init: Failed to initialize PCA9685 driver";
    return core::ErrorCode::kInitializeError;
  }

  auto config = LoadConfig(app_path + "etc/config.json");
  if (!config.has_value()) {
    logger_.LogError() << "ServoController.Init: unable to load servo configuration";
    return core::ErrorCode::kInitializeError;
  }
  servo_db_ = std::move(config.value());
  auto init_status = InitializeServosToDefault();
  if (init_status != core::ErrorCode::kOk) {
    return init_status;
  }

  for (const auto& entry : servo_db_) {
    if (entry.second.need_mosfet && gpio_ == nullptr) {
      logger_.LogWarn() << "ServoController.Init: servo " << std::to_string(static_cast<int>(entry.first)) <<
                            " requires MOSFET control but GPIO driver is null";
      return core::ErrorCode::kInitializeError;
    }
  }

  logger_.LogInfo() << "ServoController.Init: initialization completed";

  return core::ErrorCode::kOk;
}

core::ErrorCode ServoController::AutoSetServoPosition(uint8_t actuator_id,
                                                      uint8_t state) {
  auto it = servo_db_.find(actuator_id);
  if (it == servo_db_.end()) {
    logger_.LogWarn() << "ServoController.AutoSetServoPosition: unknown actuator " <<
                          std::to_string(static_cast<int>(actuator_id));
    return core::ErrorCode::kNotDefine;
  }
  auto& servo = it->second;
  servo.last_state = state;

  std::thread([this, actuator_id, state]() {
    this->ExecuteServoMovement(actuator_id, state);
  }).detach();
  return core::ErrorCode::kOk;
}

void ServoController::ExecuteServoMovement(const uint8_t actuator_id, const uint8_t state) {
  std::lock_guard<std::mutex> lock(servo_operation_mutex_);

  auto it = servo_db_.find(actuator_id);
  if (it == servo_db_.end()) {
    logger_.LogWarn() << "ServoController.ExecuteServoMovement: unknown actuator " <<
                          std::to_string(static_cast<int>(actuator_id));
    return;
  }
  auto& servo = it->second;

  if (servo.need_mosfet) {
    if (gpio_ == nullptr) {
      logger_.LogError() << "ServoController.ExecuteServoMovement: GPIO controller not available";
      return;
    }
    if (gpio_->SetPinValue(servo.mosfet_id, kOpenState) != core::ErrorCode::kOk) {
      logger_.LogError() << "ServoController.ExecuteServoMovement: failed to enable MOSFET " <<
                             std::to_string(static_cast<int>(servo.mosfet_id));
      return;
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(servo.timing.mosfet_power_on_delay_ms));
  }

  const uint16_t target_position =
      (state == kOpenState) ? servo.on_pos : servo.off_pos;
  if (driver_->SetChannelPosition(servo.channel, target_position) !=
      core::ErrorCode::kOk) {
    logger_.LogWarn() << "ServoController.ExecuteServoMovement: failed to set PWM for actuator " <<
                          std::to_string(static_cast<int>(actuator_id));
    return;
  }

  if (servo.need_loosening) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(servo.timing.loosening_delay_ms));
    const uint16_t loosening_position =
        (state == kOpenState) ? servo.on_loosening : servo.off_loosening;
    if (driver_->SetChannelPosition(servo.channel, loosening_position) !=
        core::ErrorCode::kOk) {
      logger_.LogWarn() << "ServoController.ExecuteServoMovement: failed to set loosening PWM for actuator " <<
                            std::to_string(static_cast<int>(actuator_id));
      return;
    }
  }

  if (servo.need_mosfet) {
    auto hold_time = servo.timing.servo_move_time_ms;
    if (servo.need_loosening &&
        hold_time < servo.timing.loosening_delay_ms) {
      hold_time = servo.timing.loosening_delay_ms;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(hold_time));
    if (gpio_->SetPinValue(servo.mosfet_id, kCloseState) !=
        core::ErrorCode::kOk) {
      logger_.LogWarn() << "ServoController.ExecuteServoMovement: failed to disable MOSFET " <<
                            std::to_string(static_cast<int>(servo.mosfet_id));
    }
  }
}

std::optional<uint8_t> ServoController::ReadServoPosition(uint8_t actuator_id) {
  auto it = servo_db_.find(actuator_id);
  if (it == servo_db_.end()) {
    logger_.LogWarn() << "ServoController.ReadServoPosition: unknown actuator " <<
                          std::to_string(static_cast<int>(actuator_id));
    return std::nullopt;
  }
  auto& servo = it->second;
  return servo.last_state;
  // auto pos_opt = driver_->ReadChannelPosition(servo.channel);
  // if (!pos_opt.has_value()) {
  //   logger_.LogWarn() << "ServoController.ReadServoPosition: unable to read PWM for actuator " <<
  //                         std::to_string(static_cast<int>(actuator_id));
  //   return std::nullopt;
  // }
  // const auto current = pos_opt.value();
  // if (current == servo.on_pos ||
  //     (servo.need_loosening && current == servo.on_loosening)) {
  //   servo.last_state = kOpenState;
  //   return kOpenState;
  // }
  // if (current == servo.off_pos ||
  //     (servo.need_loosening && current == servo.off_loosening)) {
  //   servo.last_state = kCloseState;
  //   return kCloseState;
  // }
  // return servo.last_state;
}

std::optional<uint16_t> ServoController::ReadRawServoPosition(uint8_t actuator_id) {
  auto it = servo_db_.find(actuator_id);
  if (it == servo_db_.end()) {
    logger_.LogWarn() << "ServoController.ReadRawServoPosition: unknown actuator " <<
                          std::to_string(static_cast<int>(actuator_id));
    return std::nullopt;
  }
  return driver_->ReadChannelPosition(it->second.channel);
}

bool ServoController::ChangeConfigPosition(uint8_t actuator_id,
                                           uint16_t new_open_val,
                                           uint16_t new_close_val) {
  auto it = servo_db_.find(actuator_id);
  if (it == servo_db_.end()) {
    logger_.LogWarn() << "ServoController.ChangeConfigPosition: unknown actuator " <<
                          std::to_string(static_cast<int>(actuator_id));
    return false;
  }
  auto& servo = it->second;
  servo.on_pos = new_open_val;
  servo.off_pos = new_close_val;
  const uint16_t command_value =
      (servo.last_state == kOpenState) ? servo.on_pos : servo.off_pos;
  if (driver_->SetChannelPosition(servo.channel, command_value) !=
      core::ErrorCode::kOk) {
    logger_.LogWarn() << "ServoController.ChangeConfigPosition: failed to update PWM for actuator " <<
                          std::to_string(static_cast<int>(actuator_id));
    return false;
  }
  return true;
}

core::ErrorCode ServoController::InitializeServosToDefault() {
  for (auto& entry : servo_db_) {
    auto& servo = entry.second;
    EnsureTimingConsistency(&servo);
    if (driver_->SetChannelPosition(servo.channel, servo.off_pos) !=
        core::ErrorCode::kOk) {
      logger_.LogWarn() << "ServoController.InitializeServosToDefault: failed to reset actuator " <<
                            std::to_string(static_cast<int>(entry.first));
      return core::ErrorCode::kError;
    }
    servo.last_state = kCloseState;
  }
  logger_.LogInfo() << "ServoController.InitializeServosToDefault: all servos reset to default";
  return core::ErrorCode::kOk;
}

void ServoController::EnsureTimingConsistency(ServoRuntimeConfig* cfg) const {
  if (cfg->timing.mosfet_power_on_delay_ms == 0U) {
    cfg->timing.mosfet_power_on_delay_ms = kDefaultMosfetPowerOnDelayMs;
  }
  if (cfg->timing.servo_move_time_ms == 0U) {
    cfg->timing.servo_move_time_ms = kDefaultServoMoveTimeMs;
  }
  if (cfg->timing.loosening_delay_ms == 0U) {
    cfg->timing.loosening_delay_ms = kDefaultLooseningDelayMs;
  }
  if (cfg->timing.servo_move_time_ms < cfg->timing.loosening_delay_ms) {
    cfg->timing.servo_move_time_ms = cfg->timing.loosening_delay_ms;
  }
}

std::optional<std::unordered_map<uint8_t, ServoRuntimeConfig>>
ServoController::LoadConfig(const std::string& file_path) {
  std::unordered_map<uint8_t, ServoRuntimeConfig> db;
  auto parser = core::json::JsonParser::Parser(file_path);
  if (!parser.has_value()) {
    logger_.LogError() << "ServoController.LoadConfig: missing config file at " << file_path;
    return std::nullopt;
  }
  auto array = parser.value().GetArray<nlohmann::json>("servos");
  if (!array.has_value()) {
    logger_.LogError() << "ServoController.LoadConfig: missing 'servos' array in config";
    return std::nullopt;
  }

  for (auto& entry : array.value()) {
    auto servo_parser_opt = core::json::JsonParser::Parser(entry);
    if (!servo_parser_opt.has_value()) {
      logger_.LogWarn() << "ServoController.LoadConfig: invalid servo entry";
      continue;
    }
    auto servo_parser = servo_parser_opt.value();
    auto actuator_id = servo_parser.GetNumber<uint8_t>("actuator_id");
    auto channel = servo_parser.GetNumber<uint8_t>("channel");
    auto on_pos = servo_parser.GetNumber<uint16_t>("on_pos");
    auto off_pos = servo_parser.GetNumber<uint16_t>("off_pos");
    if (!(actuator_id.has_value() && channel.has_value() && on_pos.has_value() &&
          off_pos.has_value())) {
      logger_.LogWarn() << "ServoController.LoadConfig: incomplete servo description";
      continue;
    }

    ServoRuntimeConfig cfg{};
    cfg.channel = channel.value();
    cfg.on_pos = on_pos.value();
    cfg.off_pos = off_pos.value();
    auto on_loosen_opt =
        servo_parser.GetNumber<uint16_t>("on_losening_pos");
    auto off_loosen_opt =
        servo_parser.GetNumber<uint16_t>("off_losening_pos");
    cfg.on_loosening = on_loosen_opt.value_or(cfg.on_pos);
    cfg.off_loosening = off_loosen_opt.value_or(cfg.off_pos);
    cfg.need_loosening = on_loosen_opt.has_value() && off_loosen_opt.has_value();

    auto mosfet_id = servo_parser.GetNumber<uint8_t>("mosfet_id");
    if (mosfet_id.has_value()) {
      cfg.need_mosfet = true;
      cfg.mosfet_id = mosfet_id.value();
    }

    cfg.timing.mosfet_power_on_delay_ms =
        servo_parser.GetNumber<uint16_t>("mosfet_power_on_delay_ms")
            .value_or(kDefaultMosfetPowerOnDelayMs);
    cfg.timing.servo_move_time_ms =
        servo_parser.GetNumber<uint16_t>("servo_move_time_ms")
            .value_or(kDefaultServoMoveTimeMs);
    cfg.timing.loosening_delay_ms =
        servo_parser.GetNumber<uint16_t>("loosening_delay_ms")
            .value_or(kDefaultLooseningDelayMs);

    if (auto servo_delay = servo_parser.GetNumber<uint16_t>("servo_delay");
        servo_delay.has_value()) {
      cfg.timing.mosfet_power_on_delay_ms = servo_delay.value();
    }
    if (auto mosfet_delay = servo_parser.GetNumber<uint16_t>("mosfet_delay");
        mosfet_delay.has_value()) {
      if (mosfet_delay.value() > cfg.timing.mosfet_power_on_delay_ms) {
        cfg.timing.servo_move_time_ms =
            mosfet_delay.value() - cfg.timing.mosfet_power_on_delay_ms;
      } else {
        logger_.LogWarn() << "ServoController.LoadConfig: mosfet_delay <= servo_delay for actuator " <<
                              std::to_string(static_cast<int>(actuator_id.value())) <<
                              ", using default move time";
        cfg.timing.servo_move_time_ms = kDefaultServoMoveTimeMs;
      }
    }
    if (auto legacy_move = servo_parser.GetNumber<uint16_t>("servo_move_time");
        legacy_move.has_value()) {
      cfg.timing.servo_move_time_ms = legacy_move.value();
    }

    EnsureTimingConsistency(&cfg);
    cfg.last_state = kCloseState;
    db.emplace(actuator_id.value(), cfg);
    logger_.LogInfo() << "ServoController.LoadConfig: registered actuator " <<
                          std::to_string(static_cast<int>(actuator_id.value()));
  }

  return db;
}

}  // namespace service
}  // namespace srp


