/**
 * @file controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_CONTROLLER_PCA9685_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_PCA9685_CONTROLLER_HPP_

#include <unordered_map>
#include <string>

#include "mw/i2c_service/controller/i2c_controller.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
namespace simba {
namespace i2c {

struct Servo {
  uint16_t on_pos;
  uint16_t off_pos;
  uint8_t channel;
  uint8_t position{0};
  bool need_mosfet{false};
  uint8_t mosfet_id;
  bool need_loosening{false};
  uint16_t on_loosening;
  uint16_t off_loosening;
  uint8_t servo_delay{50};
  uint8_t mosfet_delay{50};
};

class PCA9685 {
 private:
  I2CController i2c_;
  gpio::GPIOController gpio_{};
  std::unordered_map<uint8_t, Servo> db_;
  std::string app_name;
  core::ErrorCode ReadConfig();
  core::ErrorCode SetServo(uint8_t channel, uint16_t pos);
 public:
  PCA9685();
  void Init(uint16_t service_id, const std::unordered_map<std::string, std::string>& parms);
  core::ErrorCode AutoSetServoPosition(uint8_t actuator_id, uint8_t state);
  std::optional<uint8_t> ReadServoPosition(uint8_t actuator_id);
};

}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_PCA9685_CONTROLLER_HPP_
