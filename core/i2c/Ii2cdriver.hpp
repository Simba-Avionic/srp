/**
 * @file Ii2cdriver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_I2C_II2CDRIVER_HPP_
#define CORE_I2C_II2CDRIVER_HPP_

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "core/common/error_code.h"

namespace simba {
namespace core {
namespace i2c {

class II2CDriver {
 public:
  virtual core::ErrorCode Init() = 0;
  virtual core::ErrorCode Ioctl(const uint8_t address, const uint16_t type = I2C_SLAVE) = 0;
  virtual core::ErrorCode Write(const std::vector<uint8_t>& RegData) = 0;
  virtual core::ErrorCode Write(const uint8_t& data) = 0;
  virtual std::optional<std::vector<uint8_t>> Read(const uint8_t size = 1) = 0;
  virtual core::ErrorCode PageWrite(std::vector<uint8_t> data) = 0;
  virtual std::optional<std::vector<uint8_t>> ReadWrite(const uint8_t& reg, const uint8_t size = 1) = 0;
};
}  // namespace i2c
}  // namespace core
}  // namespace simba

#endif  // CORE_I2C_II2CDRIVER_HPP_
