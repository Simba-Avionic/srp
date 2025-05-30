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

#include "core/i2c/i2c_driver.hpp"
#include <cstdint>

namespace srp {
namespace core {
namespace i2c {

namespace {
  const constexpr auto kPath = "/dev/i2c-2";
}

I2CDriver::I2CDriver(): i2c_logger_ {
  ara::log::LoggingMenager::GetInstance()->CreateLogger("i2c-",
  "", ara::log::LogLevel::kInfo)} {
}

I2CDriver::~I2CDriver() {
  close(this->i2cFile);
}
core::ErrorCode I2CDriver::Init() {
  if ((this->i2cFile = open(kPath, O_RDWR)) < 0) {
    return core::ErrorCode::kInitializeError;
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Ioctl(const uint8_t address, const uint16_t type) {
    if (ioctl(this->i2cFile, type, address) < 0) {
      i2c_logger_.LogWarn() << ("Cant ioctl device");
        return core::ErrorCode::kInitializeError;
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Write(const std::vector<uint8_t>& RegData) {
  for (uint16_t i = 0; i < RegData.size(); i += 2) {
    uint8_t buf[2] = {RegData[i], RegData[i + 1]};
    if (write(i2cFile, buf, 2) != 2) {
      return core::ErrorCode::kInitializeError;
    }
  }
  return core::ErrorCode::kOk;
}
core::ErrorCode I2CDriver::Write(const uint8_t& data) {
  if (write(i2cFile, &data, 1) != 1) {
    return core::ErrorCode::kConnectionError;
  }
  return core::ErrorCode::kOk;
}
std::optional<std::vector<uint8_t>> I2CDriver::Read(const uint8_t size) {
  std::vector<uint8_t> data(size);
  if (read(i2cFile, data.data(), size) != size) {
    return std::nullopt;
  }
  return data;
}
core::ErrorCode I2CDriver::PageWrite(std::vector<uint8_t> data) {
  data.insert(data.begin(), 0x00);
  if (static_cast<std::size_t>(write(i2cFile, data.data(), data.size())) != data.size()) {
    return core::ErrorCode::kInitializeError;
  }
  return core::ErrorCode::kOk;
}
std::optional<std::vector<uint8_t>> I2CDriver::ReadWrite(const uint8_t& reg, const uint8_t size) {
    if (write(i2cFile, &reg, 1) == -1) {
      i2c_logger_.LogWarn() << ("Cant select reg "+ std::to_string(static_cast<int>(reg)));
      return std::nullopt;
    }
    std::vector<uint8_t> buffer(size);
    if (read(i2cFile, buffer.data(), size) != size) {
      i2c_logger_.LogWarn() << ("Cant read data");
      return std::nullopt;
    }
    return std::vector<uint8_t>(buffer);
}
}  // namespace i2c
}  // namespace core
}  // namespace srp
