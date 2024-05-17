/**
 * @file controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndefMW_I2C_SERVICE_CONTROLLER_MAX11615_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_MAX11615_CONTROLLER_HPP_

#include <unordered_map>
#include <string>
#include <vector>
#include <future> // NOLINT

#include "mw/i2c_service/controller/i2c_controller.h"
#include "core/json/json_parser.h"
namespace simba {
namespace i2c {

class MAX11615 {
 private:
  I2CController i2c_;

 protected:
 /**
  * @brief Get the Setup Data object
  * 
  * @param reset restart config to default
  * @return uint8_t 
  */
  uint8_t GetSetupData(bool reset = false);
  /**
   * @brief Get the Config Data object
   * 
   * @param channel from 0-7
   * @return uint8_t 
   */
  uint8_t GetConfigData(uint8_t channel);

 public:
  MAX11615();
  void Init(const uint16_t &service_id, const std::unordered_map<std::string, std::string>& parms);
  /**
   * @brief Get the Adc Read object
   * 
   * @param channel  (0-7)
   * @return std::optional<uint16_t> 
   */
  std::optional<uint16_t> GetAdcRead(uint8_t channel);
};

}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_MAX11615_CONTROLLER_HPP_
