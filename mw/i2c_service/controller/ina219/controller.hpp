/**
 * @file controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MW_I2C_SERVICE_CONTROLLER_INA219_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_INA219_CONTROLLER_HPP_


#include <unordered_map>
#include <string>
#include <vector>
#include <future> // NOLINT
#include <memory>
#include <utility>

#include "mw/i2c_service/controller/i2c_controller.h"
#include "ara/log/log.h"
namespace srp {
namespace i2c {

class INA219{
 private:
  I2CController i2c_;
  const ara::log::Logger& ina219_logger;

 public:
  INA219();
  core::ErrorCode InitDriver();
  core::ErrorCode Initialize(const uint8_t ina219_address);
  std::optional<float> GetCurrent(const uint8_t ina219_address);
  std::optional<float> GetVoltage(const uint8_t ina219_address);
};

}  // namespace i2c
}  // namespace srp

#endif  // MW_I2C_SERVICE_CONTROLLER_INA219_CONTROLLER_HPP_
