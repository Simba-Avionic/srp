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
#include "core/logger/Logger.h"

namespace simba {
namespace core {
namespace i2c {

namespace {
  const constexpr char *path = "/dev/i2c-2";
}

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
std::vector<uint8_t> I2CDriver::ReadWrite(const uint8_t reg, const uint8_t size) {
  AppLogger::Warning("rozmiar bufora to:"+std::to_string(static_cast<int>(size)));
    if (write(i2cFile, &reg, 1) != 1) {
      return {};
    }
    std::vector<uint8_t> buf(size);
    if (read(i2cFile, buf.data(), size) != size) {
      return {};
    }
    AppLogger::Warning(std::to_string(static_cast<int>(buf.size())));
    return buf;
}
}  // namespace i2c
}  // namespace core
}  // namespace simba
