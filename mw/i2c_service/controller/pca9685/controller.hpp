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

#include "mw/i2c_service/controller/Ii2c_controller.h"
#include "mw/i2c_service/controller/i2c_controller.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "mw/gpio_server/controller/Igpio_controller.h"
#include "core/json/json_parser.h"
#include "ara/log/logging_menager.h"

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
  std::unique_ptr<II2CController> i2c_;
  std::unique_ptr<gpio::IGPIOController> gpio_;
  std::string app_name;
  const ara::log::Logger& pac_logger_;

 protected:
  std::unordered_map<uint8_t, Servo> db_;
  std::optional<std::unordered_map<uint8_t, Servo>> ReadConfig(std::string file_path) const;
  core::ErrorCode SetServo(uint8_t channel, uint16_t pos);
  void MosfetFunc(const uint8_t &mosfet_id, const uint8_t &mosfet_time);
  std::vector<uint8_t> GenerateData(const uint8_t &channel, const uint16_t &pos) const;
  core::ErrorCode setI2C(std::unique_ptr<II2CController> adc_);
  core::ErrorCode setGPIO(std::unique_ptr<gpio::IGPIOController> gpio);
  uint8_t CalculateFirstRegister(const uint8_t& channel);
  uint8_t CalculatePosition(const std::vector<uint8_t>& val);

 public:
  PCA9685();
  bool ChangeConfigPosition(const uint8_t& actuator_id, const uint16_t new_open_val, const uint16_t new_close_val);
  std::optional<uint16_t> ReadRawServoPosition(const uint8_t &actuator_id);
  core::ErrorCode Init(const std::string& parms,
                      std::unique_ptr<II2CController> i2c = std::make_unique<I2CController>(),
                      std::unique_ptr<gpio::IGPIOController> gpio = std::make_unique<gpio::GPIOController>());
    /**
     * @brief Allow to set servo position in Engine AUTO mode, dont use in Diag
     * 
     * @param actuator_id 
     * @param state 
     * @return core::ErrorCode 
     */
  core::ErrorCode AutoSetServoPosition(const uint8_t &actuator_id, const uint8_t &state);
  /**
   * @brief Allow to read servo position
   * 
   * @param actuator_id 
   * @return std::optional<uint8_t> 
   */
  std::optional<uint8_t> ReadServoPosition(const uint8_t &actuator_id);
};

}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_PCA9685_CONTROLLER_HPP_
