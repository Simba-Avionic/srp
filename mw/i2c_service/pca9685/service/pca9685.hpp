/**
 * @file pca9685.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_SERVO_SERVICE_PCA9685_HPP_
#define MW_I2C_SERVICE_SERVO_SERVICE_PCA9685_HPP_

#include <mutex>  // NOLINT
#include <memory>
#include <unordered_map>

#include "core/i2c/i2cdriver.h"
#include "core/json/json_parser.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"

namespace simba {
namespace i2c {

struct Servo {
  uint16_t on_pos;
  uint16_t off_pos;
  uint8_t channel;
  uint8_t position{0};
  uint8_t mosfet_id;
};

class PCA9685 {
 private:
  std::unordered_map<uint8_t, Servo> db_;
  uint16_t app_id{0};
  gpio::GPIOController gpio_{};
  core::ErrorCode ReadConfig();
  std::mutex *mtx = nullptr;
 public:
  explicit PCA9685(std::mutex *mtx);
  core::ErrorCode Init(uint16_t app_id);
  core::ErrorCode SetServoPos(uint8_t actuator_id, uint16_t position);
  core::ErrorCode SetServo(uint8_t channel, uint16_t pos);
};
}  // namespace i2c
}  // namespace simba


#endif  // MW_I2C_SERVICE_SERVO_SERVICE_PCA9685_HPP_
