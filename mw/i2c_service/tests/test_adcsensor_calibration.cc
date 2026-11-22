/**
 * @file test_adcsensor_calibration.cc
 * @brief MakeCalibration nie dzieli przez 0 (F-29)
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <cmath>
#include <limits>
#include <memory>
#include <unordered_map>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "mw/i2c_service/controller/adcsensor/controller.hpp"
#include "mw/i2c_service/controller/mock/mock_ads7828.h"

using ::testing::Return;

class CalibWrapper : public srp::i2c::ADCSensorController {
 public:
  void TestSetConfig(const std::unordered_map<uint8_t, srp::i2c::SensorConfig>& db) { setConfig(db); }
  srp::core::ErrorCode TestSetPtr(std::unique_ptr<srp::i2c::IADS7828> adc) { return setPtr(std::move(adc)); }
};

class AdcCalibrationTest : public ::testing::Test {
 protected:
  AdcCalibrationTest() {
    auto adc = std::make_unique<srp::mock::MockADS7828>();
    adc_ = adc.get();
    wrapper_.TestSetConfig({{10, {1, 100.0f, -12.0f}}});
    wrapper_.TestSetPtr(std::move(adc));
  }
  CalibWrapper wrapper_;
  srp::mock::MockADS7828* adc_;
};

TEST_F(AdcCalibrationTest, ZeroVoltageIsRejectedAndConfigKept) {
  EXPECT_CALL(*adc_, GetAdcVoltage(1)).WillOnce(Return(0.0f));
  EXPECT_FALSE(wrapper_.MakeCalibration(10).has_value());
}

TEST_F(AdcCalibrationTest, NanAndInfAreRejected) {
  EXPECT_CALL(*adc_, GetAdcVoltage(1))
      .WillOnce(Return(std::numeric_limits<float>::quiet_NaN()))
      .WillOnce(Return(std::numeric_limits<float>::infinity()));
  EXPECT_FALSE(wrapper_.MakeCalibration(10).has_value());
  EXPECT_FALSE(wrapper_.MakeCalibration(10).has_value());
}

TEST_F(AdcCalibrationTest, ValidVoltageCalibrates) {
  EXPECT_CALL(*adc_, GetAdcVoltage(1)).WillOnce(Return(2.0f));
  auto cfg = wrapper_.MakeCalibration(10);
  ASSERT_TRUE(cfg.has_value());
  EXPECT_FLOAT_EQ(cfg->a, (1.0f + 12.0f) / 2.0f);
}
