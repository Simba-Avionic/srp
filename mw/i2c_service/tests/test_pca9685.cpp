/**
 * @file test_pca9685.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <memory>
#include <tuple>
#include <vector>

#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

namespace {
constexpr uint8_t kDeviceAddress = 0x70;
}  // namespace

class TestWrapper final : public srp::i2c::PCA9685 {
 public:
  using srp::i2c::PCA9685::GenerateData;
  using srp::i2c::PCA9685::setI2C;
};

class DataCreateTest
    : public ::testing::TestWithParam<std::tuple<uint8_t, uint16_t, std::vector<uint8_t>>> {};

INSTANTIATE_TEST_SUITE_P(DataCreateTestParams, DataCreateTest,
    ::testing::Values(
        std::make_tuple(0, 0, std::vector<uint8_t>{0x6, 0x0, 0x7, 0x0, 0x8, 0x0, 0x9, 0x0}),
        std::make_tuple(2, 2048, std::vector<uint8_t>{0xe, 0x0, 0xf, 0x0, 0x10, 0x0, 0x11, 0x8})));

TEST_P(DataCreateTest, GenerateDataTest) {
  TestWrapper wrapper{};
  const auto params = GetParam();
  EXPECT_EQ(wrapper.GenerateData(std::get<0>(params), std::get<1>(params)),
            std::get<2>(params));
}

TEST(TestPCAController, SetChannelPosition) {
  TestWrapper wrapper{};
  auto mock_i2c = std::make_unique<srp::mock::MockI2CController>();
  EXPECT_CALL(*mock_i2c, Write(kDeviceAddress, ::testing::_))
      .WillOnce(::testing::Return(srp::core::ErrorCode::kOk));
  ASSERT_EQ(wrapper.setI2C(std::move(mock_i2c)), srp::core::ErrorCode::kOk);
  EXPECT_EQ(wrapper.SetChannelPosition(1, 1024), srp::core::ErrorCode::kOk);
}

TEST(TestPCAController, ReadChannelPosition) {
  TestWrapper wrapper{};
  auto mock_i2c = std::make_unique<srp::mock::MockI2CController>();
  const std::vector<uint8_t> payload{0, 0, 0x34, 0x12};
  EXPECT_CALL(*mock_i2c, Read(kDeviceAddress, 10, 4))
      .WillOnce(::testing::Return(payload));
  ASSERT_EQ(wrapper.setI2C(std::move(mock_i2c)), srp::core::ErrorCode::kOk);
  const auto result = wrapper.ReadChannelPosition(1);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0x1234);
}

TEST(TestPCAController, InitConfiguresChip) {
  TestWrapper wrapper{};
  auto mock_i2c = std::make_unique<srp::mock::MockI2CController>();
  EXPECT_CALL(*mock_i2c, Write(kDeviceAddress, ::testing::_))
      .Times(3)
      .WillRepeatedly(::testing::Return(srp::core::ErrorCode::kOk));
  auto status = wrapper.Init(std::move(mock_i2c));
  EXPECT_EQ(status, srp::core::ErrorCode::kOk);
}
