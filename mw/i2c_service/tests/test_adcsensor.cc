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
#include "core/json/json_parser.h"
#include "mw/i2c_service/controller/adcsensor/controller.hpp"
#include "core/i2c/mock/mock_i2cdriver.hpp"
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

namespace {
  static const std::string FILE_PREFIX = "mw/i2c_service/tests/test_adcsensor/";
}

class TestWrapper : public srp::i2c::ADCSensorController {
 public:
  float TestCalculateB(float R, float A, float A_MIN, float RES_MIN) {
    return this->CalculateB(R, A, A_MIN, RES_MIN);
  }
  float TestCalculateA(float R, float RES_MAX, float RES_MIN, float A_MAX, float A_MIN) {
    return this->CalculateA(R, RES_MAX, RES_MIN, A_MAX, A_MIN);
  }
  std::unordered_map<uint8_t, srp::i2c::SensorConfig> TestReadConfig(std::string path) {
    return this->ReadConfig(srp::core::json::JsonParser::Parser(path).value());
  }
  void TestSetConfig(std::unordered_map<uint8_t, srp::i2c::SensorConfig> db_) {
    this->setConfig(db_);
  }

  srp::core::ErrorCode TestSetPtr(std::unique_ptr<srp::i2c::IADS7828> adc_) {
    return this->setPtr(std::move(adc_));
  }
  srp::core::ErrorCode TestInit(const std::unordered_map<std::string, std::string>& parms,
    std::unique_ptr<srp::i2c::IADS7828> adc_) {
      return Init(parms.at("app_path"), std::move(adc_));
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
 std::unordered_map<uint8_t, srp::i2c::SensorConfig>>>{
};

// tuple<Json, expectedConfig>
INSTANTIATE_TEST_SUITE_P(TestReadConfigParams, TestReadConfig, ::testing::Values(
  std::make_tuple("t_1.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{{10, {0x1, 123, -12}}}),
        std::make_tuple("t_2.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{}),
    std::make_tuple("t_3.json", std::unordered_map<uint8_t,
                                   srp::i2c::SensorConfig>{{10, {0x1, 123, -12}}, {11, {0x2, 100, -20}}}),
    // std::make_tuple("t_12.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{}),
    std::make_tuple("t_4.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{}),
    std::make_tuple("t_5.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{{11, {0x2, 0.125, -0.075}}}),
    std::make_tuple("t_6.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{}),
    std::make_tuple("t_7.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{}),
    std::make_tuple("t_8.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{}),
    std::make_tuple("t_9.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{}),
    std::make_tuple("t_10.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{}),
    std::make_tuple("t_12.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{}),
    std::make_tuple("t_13.json", std::unordered_map<uint8_t, srp::i2c::SensorConfig>{})
));

TEST_P(TestReadConfig, ReadConfigCheck) {
    TestWrapper wrapper{};
    std::string path = FILE_PREFIX+ std::get<0>(GetParam());
    auto expectedConfig = std::get<1>(GetParam());
    auto readConfig = wrapper.TestReadConfig(path);
    if (expectedConfig.size() > 0) {
      for (const auto& inst : readConfig) {
        EXPECT_EQ(expectedConfig[inst.first].channel, inst.second.channel);
        EXPECT_EQ(expectedConfig[inst.first].a, inst.second.a);
        EXPECT_EQ(expectedConfig[inst.first].b, inst.second.b);
      }
    } else {
      EXPECT_EQ(readConfig.size(), 0);
    }
}

class TestGetValue : public ::testing::TestWithParam<std::tuple<std::unordered_map<uint8_t, srp::i2c::SensorConfig>,
 uint8_t, std::optional<float>, std::optional<float>>>{
};

// tuple<config, sensor_id, mockAdcVoltage, expectedResult>
INSTANTIATE_TEST_SUITE_P(TestGetValueParams, TestGetValue, ::testing::Values(
  std::make_tuple(std::unordered_map<uint8_t, srp::i2c::SensorConfig>
    {{10, {1, 123, -12}}, {11, {1, 100, -20}}}, 11, std::optional<float>{3.2}, std::optional<float>{300}),
  std::make_tuple(std::unordered_map<uint8_t, srp::i2c::SensorConfig>
    {{10, {1, 123, -12}}, {11, {1, 100, -20}}}, 11, std::optional<float>{1}, std::optional<float>{80}),
  std::make_tuple(std::unordered_map<uint8_t, srp::i2c::SensorConfig>
    {{1, {10, 200, -40}}}, 1, std::optional<float>{3.2}, std::optional<float>{600}),
  std::make_tuple(std::unordered_map<uint8_t, srp::i2c::SensorConfig>{
    }, 1, std::optional<float>{3.2}, std::optional<float>{}),
  std::make_tuple(std::unordered_map<uint8_t, srp::i2c::SensorConfig>
    {{1, {10, 200, -40}}}, 1, std::optional<float>{}, std::optional<float>{})
));

TEST_P(TestGetValue, GetValueCheck) {
    TestWrapper wrapper{};
    auto params = GetParam();
    auto config = std::get<0>(params);
    uint8_t sensor_id = std::get<1>(params);
    auto mockAdcVoltage = std::get<2>(params);
    auto expectedResult = std::get<3>(params);
    auto mock_adc_ = std::make_unique<srp::mock::MockADS7828>();
    if (!config.empty()) {
      EXPECT_CALL(*mock_adc_, GetAdcVoltage(::testing::_)).
        WillOnce(::testing::Return(mockAdcVoltage));
    }
    wrapper.TestSetConfig(config);
    wrapper.TestSetPtr(std::move(mock_adc_));
    EXPECT_EQ(wrapper.GetValue(sensor_id), expectedResult);
}

TEST(TestADCSensor, SetPtrTest) {
    TestWrapper wrapper{};
    auto adc = std::make_unique<srp::i2c::ADS7828>();
    EXPECT_EQ(wrapper.TestSetPtr(nullptr), srp::core::ErrorCode::kInitializeError);
    EXPECT_EQ(wrapper.TestSetPtr(std::move(adc)), srp::core::ErrorCode::kOk);
}

TEST(TestADCSensor, SetInitSetPtr) {
  TestWrapper wrapper{};
  std::unordered_map<std::string, std::string> parms;
  parms["app_path"] = "123";
  auto ptr = std::unique_ptr<srp::i2c::ADS7828>();

  EXPECT_EQ(wrapper.TestInit(parms, std::move(ptr)), srp::core::ErrorCode::kInitializeError);
}

TEST(TestADCSensor, SetInitSetNullptr) {
  TestWrapper wrapper{};
  std::unordered_map<std::string, std::string> parms;
  parms["app_path"] = "123";
  auto ptr = std::unique_ptr<srp::i2c::ADS7828>(nullptr);

  EXPECT_EQ(wrapper.TestInit(parms, std::move(ptr)), srp::core::ErrorCode::kInitializeError);
}
TEST(TestADCSensor, SetInit) {
  TestWrapper wrapper{};
  std::unordered_map<std::string, std::string> parms;
  parms["app_path"] = "123";
  auto ptr = std::unique_ptr<srp::mock::MockADS7828>();

  EXPECT_EQ(wrapper.TestInit(parms, std::move(ptr)), srp::core::ErrorCode::kInitializeError);
}
