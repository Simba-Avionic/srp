/**
 * @file servo_state_test.cc
 * @brief ServoState: stan 2 = otwarcie na stale (F-20)
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <chrono>  // NOLINT
#include <cstdint>
#include <fstream>
#include <string>

#include "gtest/gtest.h"
#include "apps/ec/ServoService/servoController/servo_config_reader.hpp"
#include "apps/ec/ServoService/servoController/servo_state.hpp"

namespace {
using srp::service::ConfigReader;
using srp::service::ServoState;
using srp::service::ToRaw;
using srp::service::ToServoState;
using Clock = srp::service::Clock;

std::string WriteConfig(const std::string& servo) {
  const std::string path = std::string(::testing::TempDir()) + "servo_cfg_f20.json";
  std::ofstream f(path);
  f << "{\"servos\": [" << servo << "]}";
  return path;
}

void LoadWithAutoClosing(ConfigReader* reader, int auto_closing_ms) {
  reader->LoadServoConfig(WriteConfig("{\"actuator_id\": 1, \"channel\": 0, \"on_pos\": 100, \"off_pos\": 200, "
                                     "\"auto_closing_time\": " + std::to_string(auto_closing_ms) + "}"));
}
}  // namespace

TEST(ServoStateTest, ValidatesRange) {
  EXPECT_EQ(ToServoState(0), ServoState::kClosed);
  EXPECT_EQ(ToServoState(1), ServoState::kOpen);
  EXPECT_EQ(ToServoState(2), ServoState::kOpenPermanent);
  EXPECT_FALSE(ToServoState(3).has_value());
  EXPECT_FALSE(ToServoState(255).has_value());
  EXPECT_EQ(ToRaw(ServoState::kOpenPermanent), 2);
}

TEST(ServoStateTest, OpenSetsAutoClosingDeadline) {
  ConfigReader reader;
  LoadWithAutoClosing(&reader, 5000);
  const auto before = Clock::now();
  ASSERT_TRUE(reader.SetServoPosition(1, ToRaw(ServoState::kOpen)));
  const auto cfg = reader.GetServoConfig(1);
  ASSERT_TRUE(cfg.has_value());
  EXPECT_EQ(cfg->position, 1);
  EXPECT_GE(cfg->open_time_end, before + std::chrono::milliseconds(5000));
}

TEST(ServoStateTest, OpenPermanentHasNoAutoClosingDeadline) {
  ConfigReader reader;
  LoadWithAutoClosing(&reader, 5000);
  ASSERT_TRUE(reader.SetServoPosition(1, ToRaw(ServoState::kOpenPermanent)));
  const auto cfg = reader.GetServoConfig(1);
  ASSERT_TRUE(cfg.has_value());
  EXPECT_EQ(cfg->position, 2);
  EXPECT_EQ(cfg->open_time_end, Clock::time_point::min());
}

TEST(ServoStateTest, OpenThenPermanentCancelsTheDeadline) {
  ConfigReader reader;
  LoadWithAutoClosing(&reader, 5000);
  ASSERT_TRUE(reader.SetServoPosition(1, ToRaw(ServoState::kOpen)));
  ASSERT_GT(reader.GetServoConfig(1)->open_time_end, Clock::time_point::min());
  ASSERT_TRUE(reader.SetServoPosition(1, ToRaw(ServoState::kOpenPermanent)));
  EXPECT_EQ(reader.GetServoConfig(1)->open_time_end, Clock::time_point::min());
}

TEST(ServoStateTest, PermanentThenClosedKeepsClosed) {
  ConfigReader reader;
  LoadWithAutoClosing(&reader, 5000);
  ASSERT_TRUE(reader.SetServoPosition(1, ToRaw(ServoState::kOpenPermanent)));
  ASSERT_TRUE(reader.SetServoPosition(1, ToRaw(ServoState::kClosed)));
  EXPECT_EQ(reader.GetServoConfig(1)->position, 0);
}
