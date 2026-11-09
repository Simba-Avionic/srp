/**
 * @copyright Copyright (c) 2026
 */
// F-18: PWM / MOSFET errors are propagated by the servo driver and the controller does not store a state that was
// not reached.
#include <gtest/gtest.h>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
// white-box access to the injection points (members are private)
#define private public
#define protected public
#include "apps/ec/ServoService/servoController/servo_controller.hpp"
#undef protected
#undef private
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

using ::testing::_;
using ::testing::Return;
using srp::service::ServoController;
using srp::service::ServoDriver;
using srp::service::ServoRuntimeConfig;

namespace {
ServoRuntimeConfig Cfg(uint16_t on, uint16_t off, uint8_t ch = 3) {
  ServoRuntimeConfig c{on, off, ch};
  return c;
}

// leaked on purpose: ~ServoDriver (real GPIOController/StreamIpcSocket) segfaults in a unit-test process
ServoDriver* DriverWithI2c(srp::core::ErrorCode write_result) {
  auto* d = new ServoDriver;
  auto mock = std::make_unique<srp::mock::MockI2CController>();
  EXPECT_CALL(*mock, Write(_, _)).WillRepeatedly(Return(write_result));
  ::testing::Mock::AllowLeak(mock.get());
  EXPECT_EQ(d->driver_.setI2C(std::move(mock)), srp::core::ErrorCode::kOk);
  return d;
}

ServoController* ControllerWithI2c(srp::core::ErrorCode write_result) {
  const std::string path = std::string(getenv("TEST_TMPDIR")) + "/servo_cfg.json";
  std::ofstream(path) << R"({"servos": [{"actuator_id": 60, "channel": 3, "on_pos": 0, "off_pos": 180}]})";
  auto* c = new ServoController;  // leaked, see DriverWithI2c
  c->servo_cfg_mng.LoadServoConfig(path);
  auto mock = std::make_unique<srp::mock::MockI2CController>();
  EXPECT_CALL(*mock, Write(_, _)).WillRepeatedly(Return(write_result));
  ::testing::Mock::AllowLeak(mock.get());
  EXPECT_EQ(c->servo_ctr_.driver_.setI2C(std::move(mock)), srp::core::ErrorCode::kOk);
  return c;
}
}  // namespace

TEST(ServoErrors, DriverReturnsFalseOnPwmError) {
  auto* d = DriverWithI2c(srp::core::ErrorCode::kConnectionError);
  EXPECT_FALSE(d->SetServoPosition(Cfg(131, 0), 1)) << "F-18: PCA9685 write failed but SetServoPosition returned true";
}

TEST(ServoErrors, DriverReturnsTrueOnSuccess) {
  auto* d = DriverWithI2c(srp::core::ErrorCode::kOk);
  d->config.pca9685_XO_corelation = 1.0f;
  EXPECT_TRUE(d->SetServoPosition(Cfg(131, 0), 1));
}

TEST(ServoErrors, ControllerDoesNotStoreStateOnPwmError) {
  auto* c = ControllerWithI2c(srp::core::ErrorCode::kConnectionError);
  c->servo_ctr_.config.pca9685_XO_corelation = 1.0f;
  ASSERT_EQ(c->ReadServoPosition(60).value(), 0);
  EXPECT_FALSE(c->AutoSetServoPosition(60, 1)) << "F-18: failed move reported as success";
  EXPECT_EQ(c->ReadServoPosition(60).value(), 0) << "F-18: state stored although the move failed";
}

TEST(ServoErrors, ControllerStoresStateOnSuccess) {
  auto* c = ControllerWithI2c(srp::core::ErrorCode::kOk);
  c->servo_ctr_.config.pca9685_XO_corelation = 1.0f;
  EXPECT_TRUE(c->AutoSetServoPosition(60, 1));
  EXPECT_EQ(c->ReadServoPosition(60).value(), 1);
}
