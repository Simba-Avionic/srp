/**
 * @file test_pca9685_channel.cpp
 * @brief Unit test: PCA9685 rejects channels >= 16 instead of indexing the lookup tables out of bounds (F-22)
 *
 * @copyright Copyright (c) 2025
 */
#include <memory>
#include <optional>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

using ::testing::_;
using ::testing::Return;

class Pca9685ChannelTest : public ::testing::Test {
 protected:
  Pca9685ChannelTest() {
    auto i2c = std::make_unique<srp::mock::MockI2CController>();
    i2c_ = i2c.get();
    EXPECT_CALL(*i2c_, Init(_)).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*i2c_, Write(_, _)).WillRepeatedly(Return(srp::core::ErrorCode::kOk));
    EXPECT_EQ(pca_.Init(std::move(i2c)), srp::core::ErrorCode::kOk);
  }
  srp::i2c::PCA9685 pca_;
  srp::mock::MockI2CController* i2c_;
};

TEST_F(Pca9685ChannelTest, SetChannelOutOfRangeIsRejectedWithoutBusAccess) {
  EXPECT_CALL(*i2c_, Write(_, _)).Times(0);
  EXPECT_EQ(pca_.SetChannelPosition(16, 100), srp::core::ErrorCode::kBadVariableSize);
  EXPECT_EQ(pca_.SetChannelPosition(255, 100), srp::core::ErrorCode::kBadVariableSize);
}

TEST_F(Pca9685ChannelTest, SetLastValidChannelWorks) {
  EXPECT_CALL(*i2c_, Write(0x70, _)).WillOnce(Return(srp::core::ErrorCode::kOk));
  EXPECT_EQ(pca_.SetChannelPosition(15, 100), srp::core::ErrorCode::kOk);
}

TEST_F(Pca9685ChannelTest, ReadChannelOutOfRangeReturnsNullopt) {
  EXPECT_CALL(*i2c_, Read(_, _, _)).Times(0);
  EXPECT_FALSE(pca_.ReadChannelPosition(16).has_value());
  EXPECT_FALSE(pca_.ReadChannelPosition(200).has_value());
}

TEST_F(Pca9685ChannelTest, ReadValidChannelComposesPosition) {
  EXPECT_CALL(*i2c_, Read(_, _, _))
      .WillOnce(Return(std::optional<std::vector<uint8_t>>{std::vector<uint8_t>{0, 0, 0x66, 0x01}}));
  auto pos = pca_.ReadChannelPosition(3);
  ASSERT_TRUE(pos.has_value());
  EXPECT_EQ(pos.value(), 0x166);
}
