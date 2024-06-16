/**
 * @file test_adcsensor.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Unit tests for adcsensor
 * @version 0.1
 * @date 2024-06-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include <cmath>
#include "mw/i2c_service/controller/mock/mock_ads7828.h"
#include "mw/i2c_service/controller/adcsensor/controller.hpp"

class TestWrapper : public simba::i2c::ADCSensorController {
 public:
  float TestCalculateB(float R, float A, float A_MIN, float RES_MIN) {
    return this->CalculateB(R, A, A_MIN, RES_MIN);
  }
  float TestCalculateA(float R, float RES_MAX, float RES_MIN, float A_MAX, float A_MIN) {
    return this->CalculateA(R, RES_MAX, RES_MIN, A_MAX, A_MIN);
  }
  std::unordered_map<uint8_t, simba::i2c::SensorConfig> TestReadConfig(nlohmann::json data) {
    return this->ReadConfig(data);
  }
  void TestSetConfig(std::unordered_map<uint8_t, simba::i2c::SensorConfig> db_) {
    this->setConfig(db_);
  }

  void TestSetPtr(std::unique_ptr<simba::mock::MockADS7828> adc_) {
    this->setPtr(std::move(adc_));
  }
};

TEST(TestADCSensor, CalculateTest) {
    TestWrapper wrapper{};
    constexpr float R = 160;
    constexpr float RES_MAX = 100;
    constexpr float RES_MIN = 0;
    constexpr float A_MAX = 20;
    constexpr float A_MIN = 4;
    float A = wrapper.TestCalculateA(R, RES_MAX, RES_MIN, A_MAX, A_MIN);
    float B = wrapper.TestCalculateB(R, A, A_MIN, RES_MIN);
    EXPECT_EQ(std::round(A * A_MAX /1000 * R + B), RES_MAX);
    EXPECT_EQ(std::round(A * A_MIN/1000 * R + B), RES_MIN);
    EXPECT_EQ(std::round(A * (A_MAX + A_MIN) /2000 * R + B), RES_MAX/2);
}

class TestReadConfig : public ::testing::TestWithParam<std::tuple<std::string,
 std::unordered_map<uint8_t, simba::i2c::SensorConfig>>>{
};

// tuple<Json, expectedConfig>
INSTANTIATE_TEST_SUITE_P(TestReadConfigParams, TestReadConfig, ::testing::Values(
  std::make_tuple(R"({
    "sensors": [
        {
            "actuator_id": 10,
            "channel":1,
            "a": 123,
            "b": -12
        }
        ]})", std::unordered_map<uint8_t, simba::i2c::SensorConfig>{{10, {0x1, 123, -12}}}),
        std::make_tuple(R"({
    "sensors": [
        {
            "channel":1,
            "a": 123,
            "b": -12
        }
        ]})", std::unordered_map<uint8_t, simba::i2c::SensorConfig>{}),
    std::make_tuple(R"({
    "sensors": [
        {
            "actuator_id": 10,
            "channel":1,
            "a": 123,
            "b": -12
        },
        {
            "actuator_id": 11,
            "channel":2,
            "a": 100,
            "b": -20
        }
        ]})", std::unordered_map<uint8_t, simba::i2c::SensorConfig>{{10, {0x1, 123, -12}}, {11, {0x2, 100, -20}}})
));

TEST_P(TestReadConfig, ReadConfigCheck) {
    TestWrapper wrapper{};
    auto params = GetParam();
    std::string json = std::get<0>(params);
    auto expectedConfig = std::get<1>(params);
    if (nlohmann::json::accept(json)) {
      auto readConfig = wrapper.TestReadConfig(nlohmann::json::parse(json));
      for (const auto& inst : readConfig) {
        EXPECT_EQ(expectedConfig[inst.first].channel, inst.second.channel);
        EXPECT_EQ(expectedConfig[inst.first].a, inst.second.a);
        EXPECT_EQ(expectedConfig[inst.first].b, inst.second.b);
      }
    } else {
      EXPECT_EQ(expectedConfig.empty(), 1);
    }
}

class TestGetValue : public ::testing::TestWithParam<std::tuple<std::unordered_map<uint8_t, simba::i2c::SensorConfig>,
 uint8_t, std::optional<float>, std::optional<float>>>{
};

// tuple<config, sensor_id, mockAdcVoltage, expectedResult>
INSTANTIATE_TEST_SUITE_P(TestGetValueParams, TestGetValue, ::testing::Values(
  std::make_tuple(std::unordered_map<uint8_t, simba::i2c::SensorConfig>
  {{10, {1, 123, -12}}, {11, {1, 100, -20}}}, 11, std::optional<float>{3.2}, std::optional<float>{300})
));

TEST_P(TestGetValue, GetValueCheck) {
    TestWrapper wrapper{};
    auto params = GetParam();
    auto config = std::get<0>(params);
    uint8_t sensor_id = std::get<1>(params);
    auto mockAdcVoltage = std::get<2>(params);
    auto expectedResult = std::get<3>(params);
    auto mock_adc_ = std::make_unique<simba::mock::MockADS7828>();
    if (!config.empty()) {
      EXPECT_CALL(*mock_adc_, GetAdcVoltage(::testing::_)).
        WillOnce(::testing::Return(mockAdcVoltage));
    }
    wrapper.TestSetConfig(config);
    wrapper.TestSetPtr(std::move(mock_adc_));
    EXPECT_EQ(wrapper.GetValue(sensor_id), expectedResult);
}
