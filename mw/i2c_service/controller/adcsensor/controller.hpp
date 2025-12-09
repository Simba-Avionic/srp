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
#include <memory>
#include <utility>

#include "mw/i2c_service/controller/ads7828/Iads7828.h"
#include "mw/i2c_service/controller/ads7828/controller.hpp"
#include "core/json/json_parser.h"
namespace srp {
namespace i2c {

struct SensorConfig {
  uint8_t channel;
  float a;
  float b;
};


class ADCSensorController {
 private:
  std::unique_ptr<IADS7828> adc_;
  /**
   * @brief actuator_id, config
   * 
   */
  std::unordered_map<uint8_t, SensorConfig> db_;

 protected:
  std::unordered_map<uint8_t, SensorConfig> ReadConfig(core::json::JsonParser parser) const;
  /**
   * @brief Funckja wyliczająca wyraz wolny b dla funkcji liniowej y= a*x + b
   * 
   * @param R Rezystancja rezystora pomiarowego
   * @param A współczynnik a wyliczony funkcją CalculateA
   * @param A_MIN minimalny prąd czujnika w mA
   * @param RES_MIN minimalny odczyt czujnika w jednostce docelowej
   * @return float 
   */
  float CalculateB(const float& R, const float& A, const float& A_MIN, const float& RES_MIN) const;
  /**
   * @brief Funckja wyliczająca współczynnik a dla funkcji liniowej y= a*x + b
   * 
   * @param R Rezystancja rezystora pomiarowego
   * @param RES_MAX maksymalny odczyt czujnika w jednostce docelowej
   * @param RES_MIN minimalny odczyt czujnika w jednostce docelowej
   * @param A_MAX maksymalny prąd czujnika w mA
   * @param A_MIN minimalny prąd czujnika w mA
   * @return float 
   */
  float CalculateA(const float& R, const float& RES_MAX, const float& RES_MIN,
                                                    const float& A_MAX, const float& A_MIN) const;

  /**
   * only for test purpose do not use othervise
   */
  void setConfig(const std::unordered_map<uint8_t, SensorConfig>& db_);
  core::ErrorCode setPtr(std::unique_ptr<IADS7828> adc_);

 public:
  ADCSensorController();
  core::ErrorCode Init(const std::string path, std::unique_ptr<IADS7828> adc_);
  /**
   * @brief Get the res object w jednostce docelowej
   * 
   * @param sensor_id 
   * @return std::optional<float> 
   */
  std::optional<float> GetValue(const uint8_t sensor_id) const;

  std::optional<SensorConfig> MakeCalibration(const uint8_t sensor_id);
};

}  // namespace i2c
}  // namespace srp

#endif  // MW_I2C_SERVICE_CONTROLLER_ADCSENSOR_CONTROLLER_HPP_
