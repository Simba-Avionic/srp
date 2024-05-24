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

#ifndef MW_I2C_SERVICE_CONTROLLER_ADCSENSOR_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_ADCSENSOR_CONTROLLER_HPP_

#include <unordered_map>
#include <string>
#include <vector>
#include <future> // NOLINT

#include "mw/i2c_service/controller/ads7828/controller.hpp"
#include "core/json/json_parser.h"
namespace simba {
namespace i2c {

struct SensorConfig {
  uint8_t channel;
  float a;
  float b;
};


class ADCSensorController {
 private:
  ADS7828 adc_;
  std::string app_name;
  /**
   * @brief actuator_id, config
   * 
   */
  std::unordered_map<uint8_t, SensorConfig> db_;
 protected:
  std::unordered_map<uint8_t, SensorConfig> ReadConfig(nlohmann::json data);
  float CalculateB(float R, float A, float A_MIN) const;
  float CalculateA(float R, float RES_MAX, float RES_MIN, float A_MAX, float A_MIN) const;
 public:
  ADCSensorController();
  void Init(const std::unordered_map<std::string, std::string>& parms);
  /**
   * @brief Get the res object [Bar]
   * 
   * @param sensor_id 
   * @return std::optional<float> 
   */
  std::optional<float> GetValue(const uint8_t sensor_id);
};

}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_ADCSENSOR_CONTROLLER_HPP_
