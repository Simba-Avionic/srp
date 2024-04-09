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

enum smode_t {
  AUTO = 0x00,
  MAN = 0x01
};


struct Servo {
  smode_t mode;
  uint16_t on_pos;
  uint16_t off_pos;
  uint8_t channel;
  uint16_t position{0};
  bool need_mosfet{false};
  uint8_t mosfet_id;
  uint16_t servo_delay;
  uint16_t mosfet_time;
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
  /**
   * @brief do not use without a valid reason
   * 
   * @param actuator_id 
   * @param position 
   */
  core::ErrorCode ManSetServoPosition(uint8_t actuator_id, uint16_t position);
};

}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_PCA9685_CONTROLLER_HPP_
