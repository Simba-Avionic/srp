/**
 * @file i2cdriver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CORE_I2C_I2CDRIVER_HPP_
#define CORE_I2C_I2CDRIVER_HPP_

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include "core/common/error_code.h"

namespace simba {
namespace core {
namespace i2c {

class I2CDriver{
 private:
  int i2cFile;
 public:
  core::ErrorCode Init();
  core::ErrorCode Ioctl(const uint8_t address, const uint16_t type = I2C_SLAVE);
  /**
   * @brief Function to write data to device on i2c bus
   * 
   * @param data pair ( req, data)
   * @return core::ErrorCode 
   */
  core::ErrorCode Write(const std::vector<uint8_t> RegData);
  core::ErrorCode PageWrite(std::vector<uint8_t> data);
  std::optional<std::vector<uint8_t>> ReadWrite(uint8_t reg, const uint8_t size = 1);
};
}  // namespace i2c
}  // namespace core
}  // namespace simba

#endif  // CORE_I2C_I2CDRIVER_HPP_
