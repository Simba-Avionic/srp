/**
 * @file sim_i2cdriver.cpp
 * @author Mateusz krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <cstdint>

#include "core/i2c/i2cdriver.hpp"
#include "core/logger/Logger.h"

namespace simba {
namespace core {
namespace i2c {

namespace {
  const constexpr char *path = "/dev/i2c-2";
}

core::ErrorCode I2CDriver::Init() {
  AppLogger::Info("I2C: init i2c driver");
  if ((this->i2cFile = open(path, O_RDWR)) < 0) {
    return core::ErrorCode::kInitializeError;
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Ioctl(const uint8_t address, const uint16_t type) {
  AppLogger::Info("I2C: IOCTL for id: "+ std::to_string(address));
  if (ioctl(this->i2cFile, type, address) < 0) {
    AppLogger::Warning("Cant ioctl device");
    return core::ErrorCode::kInitializeError;
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Write(const std::vector<uint8_t>& RegData) {
  std::string log;
  std::copy(RegData.begin(), RegData.end(), log.begin());
  AppLogger::Info("I2C: Write data:" + log);
  for (uint16_t i = 0; i < RegData.size(); i += 2) {
    uint8_t buf[2] = {RegData[i], RegData[i + 1]};
    if (write(i2cFile, buf, 2) != 2) {
      return core::ErrorCode::kInitializeError;
    }
  }
  return core::ErrorCode::kOk;
}
core::ErrorCode I2CDriver::Write(const uint8_t& data) {
  AppLogger::Info("I2C: Write data:" + std::to_string(data));
  if (write(i2cFile, &data, 1) != 1) {
    return core::ErrorCode::kConnectionError;
  }
  return core::ErrorCode::kOk;
}
std::optional<std::vector<uint8_t>> I2CDriver::Read(const uint8_t size) {
  AppLogger::Info("I2C: Read data req");
  std::vector<uint8_t> data(size);
  if (read(i2cFile, data.data(), size) != size) {
    return {};
  }
  return data;
}
core::ErrorCode I2CDriver::PageWrite(std::vector<uint8_t> data) {
  std::string log;
  std::copy(data.begin(), data.end(), log.begin());
  AppLogger::Info("I2C: Write data:" + log);
  data.insert(data.begin(), 0x00);
  if (write(i2cFile, data.data(), data.size()) != data.size()) {
    return core::ErrorCode::kInitializeError;
  }
  return core::ErrorCode::kOk;
}
std::optional<std::vector<uint8_t>> I2CDriver::ReadWrite(const uint8_t& reg, const uint8_t size) {
  AppLogger::Info("I2C: ReadWrite: " + std::to_string(reg) + "  to read size: " + std::to_string(size));
    if (write(i2cFile, &reg, 1) == -1) {
      AppLogger::Warning("Cant select reg "+ std::to_string(static_cast<int>(reg)));
  }
  std::vector<uint8_t> buffer(size);
  if (read(i2cFile, buffer.data(), size) != size) {
    AppLogger::Warning("Cant read data");
    return {};
  }
  return std::vector<uint8_t>(buffer);
}
}  // namespace i2c
}  // namespace core
}  // namespace simba
