/**
 * @file test_did_write.cpp
 * @brief Unit test: GpioMWDID::Write validates the payload size before reading its bytes (F-12)
 *
 * @copyright Copyright (c) 2025
 */
#include <memory>
#include <unordered_map>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define private public  // NOLINT
#include "mw/gpio_server/gpio_mw_did.h"
#undef private
#include "core/gpio/mock/mock_gpio_driver.h"

using ::testing::_;
using ::testing::Return;

class GpioDidWriteTest : public ::testing::Test {
 protected:
  GpioDidWriteTest()
      : gpio_(std::make_shared<MockGPIO>()),
        did_(ara::core::InstanceSpecifier{"/srp/mw/gpio_service/gpio_pin_did"}, gpio_,
             std::unordered_map<uint8_t, srp::mw::GpioConf>{{1, {10, srp::core::gpio::direction_t::OUT}}}) {}
  std::shared_ptr<MockGPIO> gpio_;
  srp::mw::GpioMWDID did_;
};

TEST_F(GpioDidWriteTest, EmptyPayloadIsRejectedWithoutReadingIt) {
  EXPECT_CALL(*gpio_, setValue(_, _, _)).Times(0);
  EXPECT_FALSE(did_.Write({}).HasValue());
}

TEST_F(GpioDidWriteTest, OneBytePayloadIsRejected) {
  EXPECT_CALL(*gpio_, setValue(_, _, _)).Times(0);
  EXPECT_FALSE(did_.Write({1}).HasValue());
}

TEST_F(GpioDidWriteTest, ValidPayloadSetsThePin) {
  EXPECT_CALL(*gpio_, setValue(10, 1, _)).WillOnce(Return(srp::core::ErrorCode::kOk));
  EXPECT_TRUE(did_.Write({1, 1}).HasValue());
}
