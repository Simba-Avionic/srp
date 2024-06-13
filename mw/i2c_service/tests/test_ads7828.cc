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

class TestWrapper : public simba::i2c::ADS7828 {
 public:
  simba::core::ErrorCode TestInit(std::unique_ptr<simba::mock::MockI2CController> i2c_) {
    return this->Init(std::move(i2c_));
  }
  std::optional<uint8_t> TestGetConfigData(const uint8_t& channel) {
    return this->GetConfigData(channel);
  }
  std::optional<uint16_t> TestGetAdcRawRead(const uint8_t& channel) {
    return this->GetAdcRawRead(channel);
  }
};

class GetConfigDataTest : public ::testing::TestWithParam<
std::tuple<uint8_t, std::optional<uint8_t>>>{
};


// tuple<channel, expected_response>
INSTANTIATE_TEST_SUITE_P(GetConfigDataTestParams, GetConfigDataTest, ::testing::Values(
    std::make_tuple(0, std::optional<uint8_t>{0x80}),
    std::make_tuple(1, std::optional<uint8_t>{0x80}),
    std::make_tuple(2, std::optional<uint8_t>{0xC0}),
    std::make_tuple(3, std::optional<uint8_t>{0xC0}),
    std::make_tuple(4, std::optional<uint8_t>{0x80}),
    std::make_tuple(5, std::optional<uint8_t>{0x80}),
    std::make_tuple(6, std::optional<uint8_t>{0xC0}),
    std::make_tuple(7, std::optional<uint8_t>{0xC0}),
    std::make_tuple(8, std::optional<uint8_t>{})
));

TEST_P(GetConfigDataTest, ConfigTest) {
    TestWrapper wrapper{};
    auto params = GetParam();
    uint8_t channel = std::get<0>(params);
    auto expect = std::get<1>(params);
    EXPECT_EQ(wrapper.TestGetConfigData(channel), expect);
}

class GetAdcRawReadTest : public ::testing::TestWithParam<
std::tuple<uint8_t, std::optional<std::vector<uint8_t>>, std::optional<uint16_t>>>{
};

// tuple<channel, i2cWriteReadMockRes, expected_response>
INSTANTIATE_TEST_SUITE_P(GetAdcRawReadTestParams, GetAdcRawReadTest, ::testing::Values(
    std::make_tuple(0, std::optional<std::vector<uint8_t>>{}, std::optional<uint16_t>{}),
    std::make_tuple(1, std::optional<std::vector<uint8_t>>{{0, 1, 2}}, std::optional<uint16_t>{})
));

TEST_P(GetAdcRawReadTest, AdcRawReadTest) {
    TestWrapper wrapper{};
    auto i2c_ = std::make_unique<simba::mock::MockI2CController>();
    auto params = GetParam();
    uint8_t channel = std::get<0>(params);
    auto WriteReadRes = std::get<1>(params);
    auto expected = std::get<2>(params);
    if (channel < 7) {
      EXPECT_CALL(*i2c_, WriteRead(144, 2, ::testing::_))
        .WillOnce(::testing::Return(WriteReadRes));
    }
    std::cout << "\n" << wrapper.TestInit(std::move(i2c_)) << "\n";
    EXPECT_EQ(wrapper.TestGetAdcRawRead(channel), expected);
}
