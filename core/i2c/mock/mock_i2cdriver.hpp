/**
 * @file mock_i2cdriver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_I2C_MOCK_MOCK_I2CDRIVER_HPP_
#define CORE_I2C_MOCK_MOCK_I2CDRIVER_HPP_

#include <cstdint>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "core/i2c/Ii2cdriver.h"


class MockI2C : public simba::core::II2C {
 public:
  MOCK_METHOD((simba::core::ErrorCode), init, const std::string&, (override));
  MOCK_METHOD((std::optional<std::vector<uint8_t>>), Read, (const uint8_t, const uint8_t), (override));
  MOCK_METHOD((simba::core::ErrorCode), Write, (const uint8_t, const uint8_t,
                                        std::vector<uint8_t>), (override));
  virtual ~MockI2C() = default;
};
#endif  // CORE_I2C_MOCK_MOCK_I2CDRIVER_HPP_
