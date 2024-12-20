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
#ifndef MW_I2C_SERVICE_CONTROLLER_ADS7828_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_ADS7828_CONTROLLER_HPP_

#include <unordered_map>
#include <string>
#include <vector>
#include <future> // NOLINT
#include <memory>
#include <utility>

#include "mw/i2c_service/controller/ads7828/Iads7828.h"
#include "mw/i2c_service/controller/i2c_controller.h"
#include "core/json/json_parser.h"
namespace simba {
namespace i2c {

class ADS7828 : public IADS7828 {
 private:
  std::unique_ptr<II2CController> i2c_;

 protected:
  /**
   * @brief Get the Config Data object
   * 
   * @param channel from 0-7
   * @return uint8_t 
   */
  std::optional<uint8_t> GetConfigData(const uint8_t& channel) const override;
  /**
   * @brief Get the Adc Read object
   * 
   * @param channel  (0-7)
   * @return std::optional<uint16_t> 
   */
  std::optional<uint16_t> GetAdcRawRead(const uint8_t& channel) const override;
 public:
  ADS7828();
  std::optional<float> GetAdcVoltage(const uint8_t& channel) const override;
  core::ErrorCode Init(std::unique_ptr<II2CController> i2c_ = std::make_unique<I2CController>());
};

}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_ADS7828_CONTROLLER_HPP_
