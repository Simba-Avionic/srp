/**
 * @file temp_driver_test.cc
 * @brief Wartosci 85 C / -127 C z DS18B20 nie sa pomiarami (F-27)
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "gtest/gtest.h"
#include "core/temp/temp_driver.hpp"

namespace srp {
namespace core {
namespace temp {

TEST(TempDriverTest, SentinelValuesAreInvalid) {
  EXPECT_TRUE(TempDriver::IsInvalidReading(85.0));
  EXPECT_TRUE(TempDriver::IsInvalidReading(-127.0));
}

TEST(TempDriverTest, NormalValuesAreValid) {
  for (double v : {-55.0, -126.9, 0.0, 33.33, 84.999, 85.001, 125.0}) {
    EXPECT_FALSE(TempDriver::IsInvalidReading(v)) << v;
  }
}

}  // namespace temp
}  // namespace core
}  // namespace srp
