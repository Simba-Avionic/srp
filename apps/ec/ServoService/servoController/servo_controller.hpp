/**
 * @file servo_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_CONTROLLER_HPP_
#define APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_CONTROLLER_HPP_

#include <chrono>  // NOLINT
#include <cstdint>
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ara/log/logging_menager.h"
#include "core/common/error_code.h"
#include "apps/ec/ServoService/servoController/servo_config_reader.hpp"
#include "apps/ec/ServoService/servoController/servo_driver.hpp"


namespace srp {
namespace service {
using Clock = std::chrono::high_resolution_clock;

struct Pulsing_t {
  uint8_t pulse_state;
  Clock::time_point pulse_deadline;
};

class ServoController {
 private:
  ConfigReader servo_cfg_mng;
  ServoDriver servo_ctr_;
  const ara::log::Logger& logger_;
  std::jthread closing_thread;
  std::jthread pulsing_thread;

  std::unordered_map<uint8_t, Pulsing_t> pulsing_db;
  std::mutex pulsing_mtx_;


  void closingThreadLoop(const std::stop_token& token);
  void pulsingThreadLoop(const std::stop_token& token);
 public:
  ServoController();
  void Init(const std::string& app_path);
  bool AutoSetServoPosition(const uint8_t actuator_id, const uint8_t state);
  std::optional<uint8_t> ReadServoPosition(uint8_t actuator_id);
  bool ChangeConfigPosition(uint8_t actuator_id, uint16_t new_open_val, uint16_t new_close_val);
};

}  // namespace service
}  // namespace srp

#endif  // APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_CONTROLLER_HPP_

