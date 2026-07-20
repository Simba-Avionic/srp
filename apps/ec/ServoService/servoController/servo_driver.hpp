/**
 * @file servo_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_DRIVER_HPP_
#define APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_DRIVER_HPP_

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <mutex>  // NOLINT
#include <algorithm>

#include "apps/ec/ServoService/servoController/servo_config_reader.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/i2c_service/controller/ina219/controller.hpp"
#include "ara/log/log.h"
#include "ara/log/logging_menager.h"
#include "mw/i2c_service/controller/24lc32at/eeprom_config/cfg_manager.hpp"

namespace srp {
namespace service {

namespace {  // NOLINT
  static constexpr auto kMin_angle = 0;
  static constexpr auto kMax_angle = 180;
  static constexpr uint16_t kMin_ticks = 102;
  static constexpr uint16_t kMax_ticks = 512;
  static constexpr auto kMinServoUs = 500;
  static constexpr auto kMaxServoUs = 2500;
  static constexpr auto kServoCycleUs = 20000;  // 50Hz
  static constexpr auto kPcaResolution = 4096;
  static constexpr auto kDefault_mosfet_active_time_ms = 2000;
  static constexpr uint8_t kOpenState = 1U;
  static constexpr uint8_t kCloseState = 0U;
}  // namespace

class ServoDriver {
 private:
  srp::i2c::PCA9685 driver_;
  gpio::GPIOController gpio_;
  mutable std::mutex operation_mtx;
  const ara::log::Logger& logger_;
  eeprom::EEPROM_config config;

 public:
  void Init() {
    driver_.Init();
    auto cfg_mng = eeprom::ConfigManager();
    cfg_mng.Init();
    auto cfg = cfg_mng.GetConfig();
    if (cfg.has_value()) {
      config = cfg.value();
    } else {
      ara::log::LogFatal() << "Cant get Eeprom corelation data";
      exit(1);
    }
  }
  ServoDriver():
      logger_(ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "srdr", "", ara::log::LogLevel::kWarn)) {}
  uint16_t AngleToServoTicks(const int angle) {
    if (angle < kMin_angle || angle > kMax_angle) {
        logger_.LogWarn() << "ServoController.AngleToServoTicks: Angle out of range ("
            << kMin_angle << "-" << kMax_angle << "): " << angle << ". Clamping value.";
    }
    const int safe_angle = std::min(kMax_angle, std::max(kMin_angle, angle));

    // Adding half of the max angle (kMax_angle/2) for integer rounding
    const uint16_t usPulseTime = (safe_angle*(kMaxServoUs-kMinServoUs)+(kMax_angle/2))/kMax_angle+kMinServoUs;
    const uint16_t calculatedTicks = usPulseTime*kPcaResolution/kServoCycleUs;

    if (calculatedTicks < kMin_ticks || calculatedTicks > kMax_ticks) {
      logger_.LogWarn() << "ServoController.AngleToServoTicks: Calculated value out of range ("
            << kMin_ticks << "-" << kMax_ticks << "): " << calculatedTicks << ". Clamping value.";
    }

    const uint16_t baseTicks = std::min(kMax_ticks, std::max(kMin_ticks, calculatedTicks));
    return baseTicks;
  }
  bool SetServoPosition(const srp::service::ServoRuntimeConfig& cfg, const uint8_t state) {
    std::unique_lock<std::mutex> lock(operation_mtx);
    if (cfg.mosfet_id != 0) {
      if (gpio_.SetPinValue(cfg.mosfet_id, kOpenState, 0) != core::ErrorCode::kOk) {
        logger_.LogError() << "ServoController.ExecuteServoMovement: failed to enable MOSFET " <<
                              cfg.mosfet_id;
      }
    }

    const uint16_t target_angle = ((state == kOpenState) ? cfg.on_pos : cfg.off_pos);
    const uint16_t target_position = AngleToServoTicks(target_angle) * config.pca9685_XO_corelation;

    logger_.LogDebug() << "ServoController.ExecuteServoMovement: setting actuator "
                       << "to PWM " << target_position;

    if (driver_.SetChannelPosition(cfg.channel, target_position) != core::ErrorCode::kOk) {
        logger_.LogWarn() << "ServoController.ExecuteServoMovement: failed to set PWM ";
    }
    return true;
  }

  std::optional<uint16_t> ReadServoPosition(const uint8_t channel) {
    std::unique_lock<std::mutex> lock(operation_mtx);
    return driver_.ReadChannelPosition(channel);
  }
};

}  // namespace service
}  // namespace srp
#endif  //  APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_DRIVER_HPP_
