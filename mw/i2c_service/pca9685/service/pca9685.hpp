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

#ifndef MW_I2C_SERVICE_PCA9685_SERVICE_PCA9685_HPP_
#define MW_I2C_SERVICE_PCA9685_SERVICE_PCA9685_HPP_

#include <mutex>  // NOLINT
#include <memory>
#include <utility>
#include <unordered_map>
#include <string>
#include <vector>

#include "core/json/json_parser.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "mw/i2c_service/pca9685/data/servo_hdr.hpp"
#include "core/i2c/v2/i2cdriver.hpp"

namespace simba {
namespace i2c {

struct Servo {
  smode_t mode;
  uint16_t on_pos;
  uint16_t off_pos;
  uint8_t channel;
  uint16_t position{0};
  uint8_t mosfet_id;
};

class PCA9685 {
 private:
  std::unordered_map<uint8_t, Servo> db_;
  uint16_t app_id{0};
  std::unique_ptr<gpio::GPIOController> gpio_{};
  std::unique_ptr<core::I2CDriver> i2c_{};
  core::ErrorCode ReadConfig();
  core::ErrorCode SetServo(uint8_t channel, uint16_t pos);
  std::mutex *mtx = nullptr;
 public:
  PCA9685(std::mutex *mtx, std::unique_ptr<gpio::GPIOController> gpio,
                          std::unique_ptr<core::I2CDriver> i2c);
  explicit PCA9685(std::mutex *mtx);
  core::ErrorCode Init(uint16_t app_id);
  core::ErrorCode AutoSetServoPos(uint8_t actuator_id, uint8_t position);
  core::ErrorCode ManSetServoPos(uint8_t actuator_id, uint16_t position);
};
}  // namespace i2c
}  // namespace simba


#endif  // MW_I2C_SERVICE_PCA9685_SERVICE_PCA9685_HPP_
