/**
 * @file test_ads7828.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Unit tests for ads7828
 * @version 0.1
 * @date 2024-06-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include "mw/i2c_service/controller/ads7828/controller.hpp"
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

namespace {
    constexpr uint8_t ADS7828_ADDRESS = 0x48;
    constexpr auto RES_SIZE = 2;
}

class TestWrapper : public srp::i2c::ADS7828 {
 public:
  srp::core::ErrorCode TestInit(std::unique_ptr<srp::mock::MockI2CController> i2c_) {
    return this->Init(std::move(i2c_));
  }
  std::optional<uint8_t> TestGetConfigData(const uint8_t& channel) {
    return this->GetConfigData(channel);
  }
  std::optional<uint16_t> TestGetAdcRawRead(const uint8_t& channel) {
    return this->GetAdcRawRead(channel);
  }
  std::optional<float> TestGetAdcVoltage(const uint8_t& channel) {
    return this->GetAdcVoltage(channel);
  }
};

class GetConfigDataTest : public ::testing::TestWithParam<
std::tuple<uint8_t, std::optional<uint8_t>>>{
};

TEST(ADC7828INIT, DoubleInit) {
  TestWrapper wrapper{};
  EXPECT_EQ(wrapper.TestInit(nullptr), srp::core::ErrorCode::kInitializeError);
}


// tuple<channel, expected_response>
INSTANTIATE_TEST_SUITE_P(GetConfigDataTestParams, GetConfigDataTest, ::testing::Values(
    std::make_tuple(0, std::optional<uint8_t>{0x80}),
    std::make_tuple(1, std::optional<uint8_t>{0xC0}),
    std::make_tuple(2, std::optional<uint8_t>{0x90}),
    std::make_tuple(3, std::optional<uint8_t>{0xD0}),
    std::make_tuple(4, std::optional<uint8_t>{0xA0}),
    std::make_tuple(5, std::optional<uint8_t>{0xE0}),
    std::make_tuple(6, std::optional<uint8_t>{0xB0}),
    std::make_tuple(7, std::optional<uint8_t>{0xF0}),
    std::make_tuple(8, std::optional<uint8_t>{})
));

TEST_P(GetConfigDataTest, ConfigTest) {
    TestWrapper wrapper{};
    auto params = GetParam();
    uint8_t channel = std::get<0>(params);
    auto expect = std::get<1>(params);
    EXPECT_EQ(wrapper.TestGetConfigData(channel), expect);
}

class GetAdcRawReadandVoltageTest : public ::testing::TestWithParam<
std::tuple<uint8_t, std::optional<std::vector<uint8_t>>, std::optional<uint16_t>, std::optional<float>>>{
};

// tuple<channel, i2cWriteReadMockRes, expectedRawRead, expectedVoltage>
INSTANTIATE_TEST_SUITE_P(GetAdcRawReadandVoltageTestParams, GetAdcRawReadandVoltageTest, ::testing::Values(
    std::make_tuple(0, std::optional<std::vector<uint8_t>>{}, std::optional<uint16_t>{}, std::optional<float>{}),
    std::make_tuple(1, std::optional<std::vector<uint8_t>>{{0, 1}}, std::optional<uint16_t>{1},
      std::optional<float>{0.000805664051}),
    std::make_tuple(2, std::optional<std::vector<uint8_t>>{{1, 1}}, std::optional<uint16_t>{257},
     std::optional<float>{0.207055658}),
    std::make_tuple(3, std::optional<std::vector<uint8_t>>{{10, 1}}, std::optional<uint16_t>{2561},
     std::optional<float>{2.06330562}),
    std::make_tuple(4, std::optional<std::vector<uint8_t>>{{81, 10}}, std::optional<uint16_t>{20746},
     std::optional<float>{16.7143059}),
    std::make_tuple(5, std::optional<std::vector<uint8_t>>{{192, 56}}, std::optional<uint16_t>{49208},
     std::optional<float>{39.6451149}),
    std::make_tuple(6, std::optional<std::vector<uint8_t>>{{220, 120}}, std::optional<uint16_t>{56440},
     std::optional<float>{45.4716797}),
    std::make_tuple(7, std::optional<std::vector<uint8_t>>{{255, 255}}, std::optional<uint16_t>{65535},
     std::optional<float>{52.7991943}),
    std::make_tuple(8, std::optional<std::vector<uint8_t>>{{255, 255}}, std::optional<uint16_t>{},
     std::optional<float>{})
));

TEST_P(GetAdcRawReadandVoltageTest, AdcRawReadVoltageTest) {
    TestWrapper wrapper{};
    auto i2c_ = std::make_unique<srp::mock::MockI2CController>();
    auto params = GetParam();
    uint8_t channel = std::get<0>(params);
    auto WriteReadRes = std::get<1>(params);
    auto expectedRawRead = std::get<2>(params);
    auto expectedVoltage = std::get<3>(params);
    if (channel <= 7) {
      EXPECT_CALL(*i2c_, WriteRead(ADS7828_ADDRESS, ::testing::_, RES_SIZE))
        .WillOnce(::testing::Return(WriteReadRes))
        .WillOnce(::testing::Return(WriteReadRes));
    }
    wrapper.TestInit(std::move(i2c_));
    EXPECT_EQ(wrapper.TestGetAdcRawRead(channel), expectedRawRead);
    EXPECT_EQ(wrapper.TestGetAdcVoltage(channel), expectedVoltage);
}
