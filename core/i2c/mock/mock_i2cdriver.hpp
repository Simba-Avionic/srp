/**
 * @file mock_i2cdriver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_I2C_MOCK_MOCK_I2CDRIVER_HPP_
#define CORE_I2C_MOCK_MOCK_I2CDRIVER_HPP_
#include <vector>
#include <optional>
#include "gmock/gmock.h"
#include "core/i2c/i2c_driver.hpp"
namespace srp {
namespace mock {
class I2CDriverMock : srp::core::i2c::II2CDriver {
 public:
  MOCK_METHOD(srp::core::ErrorCode, Init, ());
  MOCK_METHOD(srp::core::ErrorCode, Ioctl, (const uint8_t address, const uint16_t type));
  MOCK_METHOD(srp::core::ErrorCode, Write, (const std::vector<uint8_t>& RegData));
  MOCK_METHOD(srp::core::ErrorCode, Write, (const uint8_t& data));
  MOCK_METHOD(std::optional<std::vector<uint8_t>>, Read, (const uint8_t size));
  MOCK_METHOD(srp::core::ErrorCode, PageWrite, (std::vector<uint8_t> data));
  MOCK_METHOD(std::optional<std::vector<uint8_t>>, ReadWrite, (const uint8_t& reg, uint8_t size));
};
}  // namespace mock
}  // namespace srp

#endif  // CORE_I2C_MOCK_MOCK_I2CDRIVER_HPP_
