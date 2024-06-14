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
#include <vector>
#include <future> // NOLINT
#include <memory>

#include "mw/i2c_service/controller/i2c_controller.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "core/json/json_parser.h"
namespace simba {
namespace i2c {

struct Servo {
  uint16_t on_pos;
  uint16_t off_pos;
  uint8_t channel;
  uint8_t position{0};
  bool need_mosfet{false};
  uint8_t mosfet_id{0};
  bool need_loosening{false};
  uint16_t on_loosening{0};
  uint16_t off_loosening{0};
  uint8_t servo_delay{0};
  uint8_t mosfet_time{0};
};

class PCA9685 {
 private:
  std::unique_ptr<I2CController> i2c_;
  gpio::GPIOController gpio_;
  std::string app_name;
 protected:
  std::unordered_map<uint8_t, Servo> db_;
  std::optional<std::unordered_map<uint8_t, Servo>> ReadConfig(nlohmann::json data);
  core::ErrorCode SetServo(uint8_t channel, uint16_t pos);
  void MosfetFunc(const uint8_t &mosfet_id, const uint8_t &mosfet_time);
  std::vector<uint8_t> GenerateData(const uint8_t &channel, const uint16_t &pos);
 public:
  PCA9685();
  void Init(const std::unordered_map<std::string, std::string>& parms, std::unique_ptr<I2CController> i2c);
  core::ErrorCode AutoSetServoPosition(const uint8_t &actuator_id, const uint8_t &state);
  std::optional<uint8_t> ReadServoPosition(const uint8_t &actuator_id);
};

}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_PCA9685_CONTROLLER_HPP_
