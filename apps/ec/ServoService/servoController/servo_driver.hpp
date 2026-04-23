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

#include "apps/ec/ServoService/servoController/servo_config_reader.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/i2c_service/controller/ina219/controller.hpp"
#include "ara/log/log.h"
#include "ara/log/logging_menager.h"

namespace srp {
namespace service {

namespace {  // NOLINT
  static constexpr auto kDefault_mosfet_active_time_ms = 2000;
  static constexpr uint8_t kOpenState = 1U;
  static constexpr uint8_t kCloseState = 0U;
}

class ServoDriver {
 private:
  srp::i2c::PCA9685 driver_;
  gpio::GPIOController gpio_;
  mutable std::mutex operation_mtx;
  const ara::log::Logger& logger_;

 public:
  void Init() {
    driver_.Init();
  }
  ServoDriver():
      logger_(ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "srdr", "", ara::log::LogLevel::kWarn)) {}
  bool SetServoPosition(const srp::service::ServoRuntimeConfig& cfg, const uint8_t state) {
    std::unique_lock<std::mutex> lock(operation_mtx);
    if (cfg.mosfet_id != 0) {
      if (gpio_.SetPinValue(cfg.mosfet_id, kOpenState, kDefault_mosfet_active_time_ms) != core::ErrorCode::kOk) {
        logger_.LogError() << "ServoController.ExecuteServoMovement: failed to enable MOSFET " <<
                                std::to_string(static_cast<int>(cfg.mosfet_id));
        return false;
      }
    }

    const uint16_t target_position = (state == kOpenState) ? cfg.on_pos : cfg.off_pos;
    logger_.LogDebug() << "ServoController.ExecuteServoMovement: setting actuator "
                        << "to PWM " << std::to_string(target_position);

    if (driver_.SetChannelPosition(cfg.channel, target_position) != core::ErrorCode::kOk) {
        logger_.LogWarn() << "ServoController.ExecuteServoMovement: failed to set PWM ";
        return false;
    }
    return true;
  }

  std::optional<uint8_t> ReadServoPosition(const uint8_t channel) {
    std::unique_lock<std::mutex> lock(operation_mtx);
    return driver_.ReadChannelPosition(channel);
  }
};

}  // namespace service
}  // namespace srp
#endif  //  APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_DRIVER_HPP_
