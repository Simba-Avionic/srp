/**
 * @file servo_controller.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_CONTROLLER_HPP_
#define APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_CONTROLLER_HPP_

#include <cstdint>
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <string>
#include <unordered_map>

#include "ara/log/logging_menager.h"
#include "core/common/error_code.h"
#include "core/json/json_parser.h"
#include "mw/gpio_server/controller/Igpio_controller.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "mw/i2c_service/controller/Ii2c_controller.h"
#include "mw/i2c_service/controller/i2c_controller.h"
#include "mw/i2c_service/controller/pca9685/controller.hpp"

namespace srp {
namespace service {


struct ServoTimingConfig {
  uint16_t mosfet_power_on_delay_ms{50};
  uint16_t servo_move_time_ms{400};
  uint16_t loosening_delay_ms{50};
};

struct ServoRuntimeConfig {
  uint16_t on_pos{0};
  uint16_t off_pos{0};
  uint16_t on_loosening{0};
  uint16_t off_loosening{0};
  uint8_t channel{0};
  uint8_t last_state{0};
  bool need_mosfet{false};
  uint8_t mosfet_id{0};
  bool need_aux_power{false};
  uint8_t aux_power_id{0};
  bool need_loosening{false};
  ServoTimingConfig timing{};
};

class ServoController {
 private:
  std::unordered_map<uint8_t, ServoRuntimeConfig> servo_db_;
  std::shared_ptr<srp::i2c::PCA9685> driver_;
  std::unique_ptr<gpio::IGPIOController> gpio_;
  const ara::log::Logger& logger_;
  mutable std::mutex servo_operation_mutex_;

  std::optional<std::unordered_map<uint8_t,
                ServoRuntimeConfig>> LoadConfig(const std::string& file_path);
  core::ErrorCode InitializeServosToDefault();
  void EnsureTimingConsistency(ServoRuntimeConfig* cfg) const;
  void ExecuteServoMovement(const uint8_t actuator_id, const uint8_t state);

 public:
  ServoController();
  core::ErrorCode Init(const std::string& app_path,
                std::shared_ptr<srp::i2c::PCA9685> driver = std::make_shared<srp::i2c::PCA9685>(),
                std::unique_ptr<gpio::IGPIOController> gpio = std::make_unique<gpio::GPIOController>(),
                std::unique_ptr<srp::i2c::II2CController> i2c_impl = std::make_unique<srp::i2c::I2CController>());
  core::ErrorCode AutoSetServoPosition(uint8_t actuator_id, uint8_t state);
  std::optional<uint8_t> ReadServoPosition(uint8_t actuator_id);
  std::optional<uint16_t> ReadRawServoPosition(uint8_t actuator_id);
  bool ChangeConfigPosition(uint8_t actuator_id, uint16_t new_open_val, uint16_t new_close_val);
  std::shared_ptr<srp::i2c::PCA9685> GetDriver() const { return driver_; }
};

}  // namespace service
}  // namespace srp

#endif  // APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_CONTROLLER_HPP_

