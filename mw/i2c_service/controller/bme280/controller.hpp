/**
 * @file controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_I2C_SERVICE_CONTROLLER_BME280_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_BME280_CONTROLLER_HPP_

#include <unordered_map>
#include <string>
#include <vector>
#include <utility>

#include "mw/i2c_service/controller/i2c_controller.h"
namespace simba {
namespace i2c {


class BME280 {
 private:
  I2CController i2c_;

 public:
  BME280();
  void Init(uint16_t service_id);
  std::optional<double> GetTemperature();
  std::optional<double> GetHumidity();
  std::optional<double> GetPressure();
};

}  // namespace i2c
}  // namespace simba

#endif  //  MW_I2C_SERVICE_CONTROLLER_BME280_CONTROLLER_HPP_
