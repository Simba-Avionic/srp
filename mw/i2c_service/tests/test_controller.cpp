/**
 * @file test_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "mw/i2c_service/controller/pca9685/controller.hpp"


class TestWrapper : public simba::i2c::PCA9685 {
 public:
  std::vector<uint8_t> GetData(const uint8_t &channel, const uint16_t &pos) {
    return this->GenerateData(channel, pos);
  }
  std::optional<std::unordered_map<uint8_t, simba::i2c::Servo>> read_config(nlohmann::json data) {
    return this->ReadConfig(data);
  }
  void Init() {
    nlohmann::json data = nlohmann::json::parse(R"({
    "servos": [
        {
            "actuator_id": 1,
            "channel": 0,
            "on_pos": 1000,
            "off_pos": 2000,
            "mosfet_id": 1,
            "on_losening_pos": 1500,
            "off_losening_pos": 2500,
            "servo_delay": 10,
            "mosfet_delay": 20
        }]})");
    std::optional<std::unordered_map<uint8_t, simba::i2c::Servo>> result = this->read_config(data);
    if (!result.has_value()) {
      return;
    }
    this->db_.insert(result.value().begin(), result.value().end());
  }
  std::optional<uint8_t> read_pos(const uint8_t &actuator_id) {
    return this->ReadServoPosition(actuator_id);
  }
};

TEST(TESTCheckServoPOS, TestPOS) {
  TestWrapper wrapper{};
  wrapper.Init();
  EXPECT_FALSE(wrapper.read_pos(0).has_value());
  EXPECT_TRUE(wrapper.read_pos(1).has_value());
  EXPECT_EQ(wrapper.read_pos(1).value(),0);
}

TEST(TestPCAController, DataCreateTest) {
    TestWrapper wrapper{};
    auto res = wrapper.GetData(0, 0);
    auto res2 = wrapper.GetData(2, 2048);
    std::vector<uint8_t> vec{0, 1, 0xFE, 121, 0x6, 0, 0x7, 0, 0x8, 0, 0x9, 0};
    std::vector<uint8_t> vec2{0, 1, 0xFE, 121, 14, 0, 15, 0, 16, 0, 17, 8};
    ASSERT_EQ(vec, res);
    EXPECT_EQ(vec2, res2);
}

class PCA9685ConfigTests : public ::testing::TestWithParam<std::tuple<uint16_t, std::string, uint16_t,
            uint16_t, uint8_t, uint8_t, bool, uint8_t, bool, uint16_t, uint16_t, uint8_t, uint8_t>> {
};

INSTANTIATE_TEST_CASE_P(
  PCA9685Test123,
  PCA9685ConfigTests,
  ::testing::Values(
    std::make_tuple(1, R"({
    "servos": [
        {
            "actuator_id": 1,
            "channel": 0,
            "on_pos": 1000,
            "off_pos": 2000,
            "mosfet_id": 1,
            "on_losening_pos": 1500,
            "off_losening_pos": 2500,
            "servo_delay": 10,
            "mosfet_delay": 20
        }]})", 1000, 2000, 0, 0, true, 1, true, 1500, 2500, 10, 20),
    std::make_tuple(2, R"({
    "servos": [
        {
            "actuator_id": 2,
            "channel": 1,
            "on_pos": 1500,
            "off_pos": 2000
        }]})", 1500, 2000, 1, 0, false, 0, false, 0, 0, 0, 0),
    std::make_tuple(3, R"({
    "servos": [
        {
            "actuator_id": 3,
            "channel": 3,
            "on_pos": 1500,
            "off_pos": 2000,
            "mosfet_id":2
        }]})", 1500, 2000, 3, 0, true, 2, false, 0, 0, 50, 250),
    std::make_tuple(4, R"({
    "servos": [
        {
            "actuator_id": 4,
            "channel": 1,
            "on_pos": 1500,
            "off_pos": 2000,
            "on_losening_pos":1000,
            "off_losening_pos":2500
        }]})", 1500, 2000, 1, 0, false, 0, true, 1000, 2500, 0, 0)

));

TEST_P(PCA9685ConfigTests, READ_DATA_TEST) {
  TestWrapper pca9685_;
  nlohmann::json data = nlohmann::json::parse(std::get<1>(GetParam()));
  std::optional<std::unordered_map<uint8_t, simba::i2c::Servo>> result = pca9685_.read_config(data);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  auto servo_map = result.value();
  EXPECT_TRUE(servo_map.find(std::get<0>(GetParam())) != servo_map.end());
  const simba::i2c::Servo& servo = servo_map.at(std::get<0>(GetParam()));
  EXPECT_EQ(servo.on_pos, std::get<2>(GetParam()));
  EXPECT_EQ(servo.off_pos, std::get<3>(GetParam()));
  EXPECT_EQ(servo.channel, std::get<4>(GetParam()));
  EXPECT_EQ(servo.position, std::get<5>(GetParam()));
  EXPECT_EQ(servo.need_mosfet, std::get<6>(GetParam()));
  EXPECT_EQ(servo.mosfet_id, std::get<7>(GetParam()));
  EXPECT_EQ(servo.need_loosening, std::get<8>(GetParam()));
  EXPECT_EQ(servo.on_loosening, std::get<9>(GetParam()));
  EXPECT_EQ(servo.off_loosening, std::get<10>(GetParam()));
  EXPECT_EQ(servo.servo_delay, std::get<11>(GetParam()));
  EXPECT_EQ(servo.mosfet_time, std::get<12>(GetParam()));
};