// SDO is connected to GND
// Slave adress -> 0x76


// 10 / second -> t_stop ~ 100ms?? (62.5 eeventually)
// normal mode

/**
 * @file controller.hpp
 * @author Grzegorz Horbaczewski (gregority9@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef MW_I2C_SERVICE_CONTROLLER_BME280_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_BME280_CONTROLLER_HPP_

#include <string>
#include <memory>
#include <optional>
#include <vector>

#include "mw/i2c_service/controller/Ii2c_controller.h"
#include "mw/i2c_service/controller/i2c_controller.h"
#include "ara/log/logging_menager.h"

namespace srp {
namespace i2c {

struct trimmingParameters {
  // Temperatura
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;

  // Ciśnienie
  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;

  // Wilgotność
  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;
};

class BME280 {
 private:
  std::unique_ptr<II2CController> i2c_;
  const ara::log::Logger& pac_logger_;
  long t_fine;
  trimmingParameters param;


 protected:
  core::ErrorCode setI2C(std::unique_ptr<II2CController> adc_);
  std::optional<std::vector<uint8_t>> readOutputData();
  std::optional<int32_t> extractBits(const std::optional<std::vector<uint8_t>>& input, uint8_t startBit, uint8_t length);


 private:
  core::ErrorCode InitializeBME();
  core::ErrorCode readTrimmingParameters();

 public:
  BME280();
  core::ErrorCode Init(std::unique_ptr<II2CController> i2c = std::make_unique<I2CController>());
  uint32_t getPressure();
  int32_t getTemperature();
  uint32_t getHumidity();
};

}  // namespace i2c
}  // namespace srp

#endif  // MW_I2C_SERVICE_CONTROLLER_BME280_CONTROLLER_HPP_
