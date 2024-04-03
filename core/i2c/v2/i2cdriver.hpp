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

#ifndef CORE_I2C_V2_I2CDRIVER_HPP_
#define CORE_I2C_V2_I2CDRIVER_HPP_

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <utility>
#include "core/common/error_code.h"

namespace simba {
namespace core {

class I2CDriver{
 private:
  int i2cFile;
 public:
  core::ErrorCode Init();
  core::ErrorCode Ioctl(const uint8_t address, const uint16_t type = I2C_SLAVE);
                                          //          req  ,   data
  core::ErrorCode Write(const std::vector<std::pair<uint8_t, uint8_t>> data);
  void Read(){}
};
}  // namespace core
}  // namespace simba

#endif  // CORE_I2C_V2_I2CDRIVER_HPP_
