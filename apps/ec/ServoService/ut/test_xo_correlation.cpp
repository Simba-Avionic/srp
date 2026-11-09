/**
 * @copyright Copyright (c) 2026
 */
// F-23: pca9685_XO_corelation is valid only in 0.7 - 1.3, everything else (and NaN) falls back to 1.0.
#include <gtest/gtest.h>
#include <limits>
#include "apps/ec/ServoService/servoController/servo_driver.hpp"

using srp::service::ServoDriver;

TEST(XoCorrelation, OutOfRangeFallsBackToOne) {
  const float bad[] = {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(), 0.0f, -1.0f,
                       0.69f, 1.31f, 8.0f};
  for (float v : bad) {
    EXPECT_EQ(ServoDriver::SanitizeXoCorrelation(v), 1.0f) << "value " << v;
  }
}

TEST(XoCorrelation, InRangeIsKept) {
  const float good[] = {0.7f, 1.0f, 1.0429f, 1.3f};
  for (float v : good) {
    EXPECT_EQ(ServoDriver::SanitizeXoCorrelation(v), v) << "value " << v;
  }
}
