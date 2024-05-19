/**
 * @file controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_CONTROLLER_PRESSURE_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_PRESSURE_CONTROLLER_HPP_

#include <unordered_map>
#include <string>
#include <vector>
#include <future> // NOLINT

#include "mw/i2c_service/controller/ads7828/controller.hpp"
#include "core/json/json_parser.h"
namespace simba {
namespace i2c {

struct PressureSensorConfig {
  uint8_t channel;
  uint8_t a;
  uint8_t b;
};


class PressureController {
 private:
  ADS7828 adc_;
  std::string app_name;
  /**
   * @brief actuator_id, config
   * 
   */
  std::unordered_map<uint8_t, PressureSensorConfig> db_;
 protected:
  std::unordered_map<uint8_t, PressureSensorConfig> ReadConfig(nlohmann::json data);
 public:
  PressureController();
  void Init(const uint16_t &service_id, const std::unordered_map<std::string, std::string>& parms);
  std::optional<float> GetPressure(const uint8_t sensor_id);
};

}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_PRESSURE_CONTROLLER_HPP_
