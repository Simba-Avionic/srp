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
namespace simba {
namespace mock {
class I2CDriverMock : simba::core::i2c::II2CDriver {
 public:
  MOCK_METHOD(simba::core::ErrorCode, Init, ());
  MOCK_METHOD(simba::core::ErrorCode, Ioctl, (const uint8_t address, const uint16_t type));
  MOCK_METHOD(simba::core::ErrorCode, Write, (const std::vector<uint8_t>& RegData));
  MOCK_METHOD(simba::core::ErrorCode, Write, (const uint8_t& data));
  MOCK_METHOD(std::optional<std::vector<uint8_t>>, Read, (const uint8_t size));
  MOCK_METHOD(simba::core::ErrorCode, PageWrite, (std::vector<uint8_t> data));
  MOCK_METHOD(std::optional<std::vector<uint8_t>>, ReadWrite, (const uint8_t& reg, uint8_t size));
};
}  // namespace mock
}  // namespace simba

#endif  // CORE_I2C_MOCK_MOCK_I2CDRIVER_HPP_
