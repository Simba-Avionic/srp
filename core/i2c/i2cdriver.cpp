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

#include "core/i2c/i2cdriver.hpp"

#define path "/dev/i2c-2"

namespace simba {
namespace core {
namespace i2c {

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

core::ErrorCode I2CDriver::Write(const std::vector<uint8_t> RegData) {
    for (int i = 0; i < RegData.size(); i+=2) {
      uint8_t buf[2] = {RegData[i], RegData[i+1]};
      if (write(i2cFile, buf, 2) != 2) {
        return core::ErrorCode::kInitializeError;
      }
    }
    return core::ErrorCode::kOk;
}
core::ErrorCode I2CDriver::PageWrite(std::vector<uint8_t> data) {
    data.insert(data.begin(), 0x00);
    if (write(i2cFile, data.data(), data.size()) != data.size()) {
      return core::ErrorCode::kInitializeError;
    }
    return core::ErrorCode::kOk;
}
std::optional<uint8_t> I2CDriver::Read(const uint8_t address, const uint8_t reg, const uint8_t len) {
  uint8_t buf;
  if (read(i2cFile, &buf, len) != len) {
    return {};
  }
  return buf;
}
std::vector<uint8_t> I2CDriver::PageRead(const uint8_t address, const uint8_t startReg, const uint8_t len) {
  std::vector<uint8_t> readBuf(len);
  if (read(i2cFile, readBuf.data(), len) != len) {
    return {};
  }
  return readBuf;
}
}  // namespace i2c
}  // namespace core
}  // namespace simba
