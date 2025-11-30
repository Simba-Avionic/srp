/**
 * @file controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 24LC32AT EEPROM Controller
 * @version 0.1
 * @date 2024-12-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_CONTROLLER_24LC32AT_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_24LC32AT_CONTROLLER_HPP_

#include <string>
#include <memory>
#include <optional>
#include <vector>

#include "mw/i2c_service/controller/Ii2c_controller.h"
#include "mw/i2c_service/controller/i2c_controller.h"
#include "ara/log/logging_menager.h"

namespace srp {
namespace i2c {

class EEPROM24LC32AT {
 private:
  std::unique_ptr<II2CController> i2c_;
  const ara::log::Logger& eeprom_logger_;
  uint8_t device_address_;

 protected:
  std::vector<uint8_t> GenerateAddressBytes(uint16_t address) const;
  core::ErrorCode WaitForWriteCycle() const;
  core::ErrorCode setI2C(std::unique_ptr<II2CController> i2c);

 private:
  core::ErrorCode InitializeEEPROM();

 public:
  explicit EEPROM24LC32AT(uint8_t address = 0x50);
  core::ErrorCode Init(std::unique_ptr<II2CController> i2c = std::make_unique<I2CController>());
  core::ErrorCode WriteByte(uint16_t address, uint8_t data);
  core::ErrorCode WritePage(uint16_t address, const std::vector<uint8_t>& data);
  std::optional<uint8_t> ReadByte(uint16_t address);
  std::optional<std::vector<uint8_t>> ReadSequential(uint16_t address, uint8_t size);
};

}  // namespace i2c
}  // namespace srp

#endif  // MW_I2C_SERVICE_CONTROLLER_24LC32AT_CONTROLLER_HPP_

