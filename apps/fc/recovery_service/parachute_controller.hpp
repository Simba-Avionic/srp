/**
 * @file parachute_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_FC_RECOVERY_SERVICE_PARACHUTE_CONTROLLER_HPP_
#define APPS_FC_RECOVERY_SERVICE_PARACHUTE_CONTROLLER_HPP_
#include <stop_token>
#include <atomic>
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <string>
#include <thread>  // NOLINT

#include "apps/ec/ServoService/servoController/servo_controller.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
namespace srp {
namespace apps {
namespace recovery {

enum ParachuteState_t {
  CLOSED = 1,
  OPENING_REEFED,
  OPEN_REEFED,
  OPENING_UNREEFED,
  OPEN_UNREEFED,
  ERROR,
};

struct Parachute_config_t {
  uint16_t mosfet_delay;
  uint16_t Servo_move_time;
  uint16_t Linecutter_active_time;
  uint16_t Linecutter_inactive_time;
  uint16_t backup_linecutter_activation_time;
  uint16_t linecutter_active_height;
  uint8_t servo_mosfet_id;
  uint8_t linecutter_pin_id;
  uint8_t Recovery_servo_id;
  uint8_t Servo_sequence_num;
  uint8_t Linecutter_sequence_num;
};

class ParachuteController {
 private:
  srp::service::ServoController servo_controller;
  gpio::GPIOController gpio_controller;
  std::atomic<ParachuteState_t> parachute_state;
  std::atomic<bool> operation_in_progress_;
  std::mutex operation_mtx_;
  std::mutex config_mtx_;
  Parachute_config_t config_;
  std::jthread operation_thread_;

  std::optional<Parachute_config_t> read_config(std::optional<srp::core::json::JsonParser> parser_);
  void JoinFinishedWorkerIfNeeded();

 public:
  static std::shared_ptr<ParachuteController> GetInstance();
  ParachuteState_t GetParachuteState() const noexcept { return parachute_state.load(); }
  bool OpenParachute(const std::stop_token& token);
  bool UnreefParachute(const std::stop_token& token);
  void StopThred();
  void Init(const std::string& path);
  ParachuteController();
  ParachuteController(const ParachuteController&) = delete;
  ParachuteController& operator=(const ParachuteController&) = delete;
  ParachuteController(ParachuteController&&) = delete;
  ParachuteController& operator=(ParachuteController&&) = delete;
};

}  // namespace recovery
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RECOVERY_SERVICE_PARACHUTE_CONTROLLER_HPP_
