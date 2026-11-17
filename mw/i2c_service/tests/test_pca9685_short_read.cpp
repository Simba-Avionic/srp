/**
 * @file test_pca9685_short_read.cpp
 * @brief Unit test: ReadChannelPosition rejects a response shorter than 4 bytes (F-40)
 *
 * @copyright Copyright (c) 2026
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

class Pca9685ShortReadTest : public ::testing::Test {
 protected:
  Pca9685ShortReadTest() {
    auto i2c = std::make_unique<srp::mock::MockI2CController>();
    i2c_ = i2c.get();
    EXPECT_CALL(*i2c_, Init(_)).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_EQ(pca_.Init(std::move(i2c)), srp::core::ErrorCode::kOk);
  }
  srp::i2c::PCA9685 pca_;
  srp::mock::MockI2CController* i2c_;
};

TEST_F(Pca9685ShortReadTest, EmptyAndShortResponsesGiveNullopt) {
  for (size_t n = 0; n < 4; ++n) {
    EXPECT_CALL(*i2c_, Read(_, _, _))
        .WillOnce(Return(std::optional<std::vector<uint8_t>>(std::vector<uint8_t>(n, 0x11))));
    EXPECT_FALSE(pca_.ReadChannelPosition(0).has_value()) << "size " << n;
  }
}

TEST_F(Pca9685ShortReadTest, NulloptResponseGivesNullopt) {
  EXPECT_CALL(*i2c_, Read(_, _, _)).WillOnce(Return(std::nullopt));
  EXPECT_FALSE(pca_.ReadChannelPosition(0).has_value());
}

TEST_F(Pca9685ShortReadTest, FullResponseComposesPosition) {
  EXPECT_CALL(*i2c_, Read(_, _, _))
      .WillOnce(Return(std::optional<std::vector<uint8_t>>(std::vector<uint8_t>{0, 0, 0x66, 0x01})));
  auto pos = pca_.ReadChannelPosition(0);
  ASSERT_TRUE(pos.has_value());
  EXPECT_EQ(pos.value(), 0x166);
}
