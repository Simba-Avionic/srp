/**
 * @file controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_CONTROLLER_24AA01_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_24AA01_CONTROLLER_HPP_

#include <unordered_map>
#include <string>
#include <vector>

#include "mw/i2c_service/controller/i2c_controller.h"
namespace simba {
namespace i2c {


class EEPROM {
 private:
  I2CController i2c_;

 public:
  EEPROM();
  void Init(uint16_t service_id);
  std::vector<uint8_t> Read();
  std::vector<uint8_t> PageRead(std::vector<uint8_t> reg);
  /**
   * @brief zapisa doeeprom
   * 
   * @param regData (na zmiane nr rejestru i dane do zapisania)
   * @return core::ErrorCode 
   */

  core::ErrorCode Write(std::vector<uint8_t> data);
  /**
   * @brief zapis do eeprom
   * 
   * @param data same dane, zapisywane od rejestru 0x00
   * @return core::ErrorCode 
   */
  core::ErrorCode PageWrite(std::vector<uint8_t> data);
};

}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_24AA01_CONTROLLER_HPP_
