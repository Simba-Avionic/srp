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
#include <memory>
#include <string>
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
namespace srp {
namespace apps {
namespace recovery {

struct Parachute_config_t {
  uint8_t Linecutter_pin_id;
  uint8_t Recovery_servo_id;
  uint16_t Servo_move_time;
  uint8_t Servo_sequence_num;
  uint8_t Linecutter_sequence_num;
  uint16_t Linecutter_active_time;
  uint16_t Linecutter_inactive_time;
};

class ParachuteController {
 private:
  std::unique_ptr<i2c::PCA9685> servo_controller;
  std::unique_ptr<gpio::GPIOController> gpio_controller;
  bool parachute_unreefed;
  bool parachute_open;
  Parachute_config_t config_;

  std::optional<Parachute_config_t> read_config(std::optional<srp::core::json::JsonParser> parser_);
 public:
  bool OpenParachute(bool diag = false);
  bool UnreefParachute(bool diag = false);
  void Init(std::unique_ptr<i2c::PCA9685>&& servo,
              std::unique_ptr<gpio::GPIOController>&& gpio, const std::string& path);
  ParachuteController();
};

}  // namespace recovery
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RECOVERY_SERVICE_PARACHUTE_CONTROLLER_HPP_
