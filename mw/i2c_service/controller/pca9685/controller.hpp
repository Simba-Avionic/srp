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

#include <string>
#include <memory>
#include <optional>
#include <vector>

#include "mw/i2c_service/controller/Ii2c_controller.h"
#include "mw/i2c_service/controller/i2c_controller.h"
#include "ara/log/logging_menager.h"

namespace srp {
namespace i2c {

class PCA9685 {
 private:
  std::unique_ptr<II2CController> i2c_;
  const ara::log::Logger& pac_logger_;

 protected:
  std::vector<uint8_t> GenerateData(const uint8_t &channel, const uint16_t &pos) const;
  core::ErrorCode setI2C(std::unique_ptr<II2CController> adc_);
  uint16_t ComposePosition(const std::vector<uint8_t>& val) const;

 private:
  core::ErrorCode InitializePCA();

 public:
  PCA9685();
  core::ErrorCode Init(std::unique_ptr<II2CController> i2c = std::make_unique<I2CController>());
  core::ErrorCode SetChannelPosition(uint8_t channel, uint16_t pos);
  std::optional<uint16_t> ReadChannelPosition(const uint8_t &channel);
};

}  // namespace i2c
}  // namespace srp

#endif  // MW_I2C_SERVICE_CONTROLLER_PCA9685_CONTROLLER_HPP_
