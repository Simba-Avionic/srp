/**
 * @copyright Copyright (c) 2025
 */
// Audit tests (bugs): assert the CORRECT behaviour, RED on the current code.
#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>  // NOLINT
#include <cmath>
#include <condition_variable>  // NOLINT
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <string>
#include <thread>  // NOLINT
#include <vector>
#include <unordered_map>
#include <atomic>
#include <functional>
#include <limits>
// white-box access to ServoDriver::driver_/config (members are private, no injection point)
#define private public
#define protected public
#include "apps/ec/ServoService/servoController/servo_driver.hpp"
#undef protected
#undef private
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using srp::service::ServoDriver;
using srp::service::ServoRuntimeConfig;

namespace {
ServoRuntimeConfig Cfg(uint16_t on, uint16_t off, uint8_t ch = 3) {
  ServoRuntimeConfig c{on, off, ch};
  return c;
}
}  // namespace

/**
 * F-18 (wysoka) apps/ec/ServoService/servoController/servo_driver.hpp:~87-106 (SetServoPosition).
 * I2C/PCA9685 write error is only logged and SetServoPosition() still returns true; ServoController then
 * reports the commanded state as the real one (servo_controller.cpp:139-144).
 * Expected: false when SetChannelPosition fails. Actual: true.
 */
TEST(AuditBugs, F18_ServoPositionReportsSuccessOnI2cError) {
  // leaked on purpose: ~ServoDriver (real GPIOController/StreamIpcSocket) segfaults in a unit-test process
  auto& d = *new ServoDriver;
  auto mock = std::make_unique<srp::mock::MockI2CController>();
  EXPECT_CALL(*mock, Write(_, _)).WillRepeatedly(Return(srp::core::ErrorCode::kConnectionError));
  ASSERT_EQ(d.driver_.setI2C(std::move(mock)), srp::core::ErrorCode::kOk);
  EXPECT_FALSE(d.SetServoPosition(Cfg(131, 0), 1))
      << "F-18: SetServoPosition returned true although PCA9685 write failed (servo_driver.hpp:87-106)";
}

/**
 * F-23 (srednia) servo_driver.hpp:98 + eeprom_config/cfg_manager.hpp:26.
 * pca9685_XO_corelation read from EEPROM is multiplied into the PWM ticks without validation.
 * Expected: for NaN / 0 / -1 / 8.1 either an error (false) or a PWM value within 1..4095 (here also >= 102).
 * Actual: 0, 65024, 4147 (>4095 sets the PCA "full off" bit) or UB for NaN.
 */
TEST(AuditBugs, F23_InvalidXoCorrelationProducesInvalidPwm) {
  const float bad[] = {std::numeric_limits<float>::quiet_NaN(), 0.0f, -1.0f, 8.1f};
  for (float corr : bad) {
    auto& d = *new ServoDriver;  // leaked on purpose, see F18
    d.config.pca9685_XO_corelation = corr;
    auto mock = std::make_unique<srp::mock::MockI2CController>();
    std::vector<uint8_t> payload;
    EXPECT_CALL(*mock, Write(_, _)).WillRepeatedly(Invoke([&](const uint8_t, const std::vector<uint8_t> v) {
      payload = v;
      return srp::core::ErrorCode::kOk;
    }));
    ASSERT_EQ(d.driver_.setI2C(std::move(mock)), srp::core::ErrorCode::kOk);
    const bool ok = d.SetServoPosition(Cfg(131, 0), 0);  // close -> 0 deg -> 102 ticks * corr
    ASSERT_EQ(payload.size(), 8u);
    const uint16_t off = payload[5] | (static_cast<uint16_t>(payload[7]) << 8);
    EXPECT_TRUE(!ok || (off >= 1 && off <= 4095))
        << "F-23: pca9685_XO_corelation=" << corr << " gives OFF=" << off << " and SetServoPosition returned true"
        << " (servo_driver.hpp:98, no validation of the EEPROM float)";
  }
}

