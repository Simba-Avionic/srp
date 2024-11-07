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
#include "ara/log/log.h"
#include <cstdint>

namespace simba {
namespace core {
namespace i2c {

namespace {
  const constexpr char *path = "/dev/i2c-2";
}

I2CDriver::I2CDriver(): i2c_logger_ {
  ara::log::LoggingMenager::GetInstance()->CreateLogger("I2C-",
  "", ara::log::LogLevel::kDebug)} {
}

I2CDriver::~I2CDriver() {
  close(this->i2cFile);
}
core::ErrorCode I2CDriver::Init() {
  ara::log::LogInfo() << "Initialize I2C";
  return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Ioctl(const uint8_t address, const uint16_t type) {
    ara::log::LogInfo() << "IOCTL ADDRESS:" << address << "type:" << type;
    return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Write(const std::vector<uint8_t>& RegData) {
  ara::log::LogInfo() << "Write Data:" << RegData;
  return core::ErrorCode::kOk;
}
core::ErrorCode I2CDriver::Write(const uint8_t& data) {
  ara::log::LogInfo() << "Write Data:" << data;
  return core::ErrorCode::kOk;
}
std::optional<std::vector<uint8_t>> I2CDriver::Read(const uint8_t size) {
  ara::log::LogInfo() << "READ Data size:" << size;
  return std::nullopt;
}
core::ErrorCode I2CDriver::PageWrite(std::vector<uint8_t> data) {
  ara::log::LogInfo() << "PageWrite Data:" << data;
  return core::ErrorCode::kOk;
}
std::optional<std::vector<uint8_t>> I2CDriver::ReadWrite(const uint8_t& reg, const uint8_t size) {
    ara::log::LogInfo() << "READ_WRITE from reg:" << reg << ", size:" << size;
    return std::nullopt;
}  // namespace i2c
}  // namespace core
}  // namespace simba
