/**
 * @file i2cdriver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "core/i2c/v2/i2cdriver.hpp"

#define path "/dev/i2c-2"

namespace simba {
namespace core {

core::ErrorCode I2CDriver::Init() {
    if ((this->i2cFile = open(path, O_RDWR)) < 0) {
      return core::ErrorCode::kInitializeError;
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Ioctl(const uint8_t address, const uint16_t type) {
    if (ioctl(this->i2cFile, type, address) < 0) {
        return core::ErrorCode::kInitializeError;
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Write(const std::vector<std::pair<uint8_t, uint8_t>> data) {
    for (auto d : data) {
      uint8_t buf[2] = {d.first, d.second};
      if (write(i2cFile, buf, 2) != 2) {
        return core::ErrorCode::kInitializeError;
      }
    }
    return core::ErrorCode::kOk;
}

}  // namespace core
}  // namespace simba
