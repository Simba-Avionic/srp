/**
 * @file test_config_channel.cpp
 * @brief Unit test: ConfigReader rejects servo entries with a channel outside 0-15 (F-22)
 *
 * @copyright Copyright (c) 2025
 */
#include <cstdint>
#include <fstream>
#include <string>

#include "gtest/gtest.h"

#include "apps/ec/ServoService/servoController/servo_config_reader.hpp"

namespace {
std::string WriteConfig(const std::string& servos) {
  const std::string path = std::string(::testing::TempDir()) + "servo_cfg_f22.json";
  std::ofstream f(path);
  f << "{\"servos\": [" << servos << "]}";
  return path;
}
std::string Servo(int id, int64_t channel) {
  return "{\"actuator_id\": " + std::to_string(id) + ", \"channel\": " + std::to_string(channel) +
         ", \"on_pos\": 100, \"off_pos\": 200}";
}
}  // namespace

TEST(ServoConfigChannel, ValidChannelsAreLoaded) {
  srp::service::ConfigReader reader;
  reader.LoadServoConfig(WriteConfig(Servo(1, 0) + "," + Servo(2, 15)));
  EXPECT_TRUE(reader.GetServoConfig(1).has_value());
  EXPECT_TRUE(reader.GetServoConfig(2).has_value());
}

TEST(ServoConfigChannel, ChannelOutOfRangeIsSkipped) {
  srp::service::ConfigReader reader;
  reader.LoadServoConfig(WriteConfig(Servo(1, 16) + "," + Servo(2, 255) + "," + Servo(3, -1) + "," + Servo(4, 3)));
  EXPECT_FALSE(reader.GetServoConfig(1).has_value());
  EXPECT_FALSE(reader.GetServoConfig(2).has_value());
  EXPECT_FALSE(reader.GetServoConfig(3).has_value());
  EXPECT_TRUE(reader.GetServoConfig(4).has_value());
}

TEST(ServoConfigChannel, WrappingChannelIsNotMistakenForAValidOne) {
  // 257 would wrap to 1 in uint8_t
  srp::service::ConfigReader reader;
  reader.LoadServoConfig(WriteConfig(Servo(1, 257)));
  EXPECT_FALSE(reader.GetServoConfig(1).has_value());
}
