/**
 * @copyright Copyright (c) 2025
 */
// Audit tests (bugs): assert the CORRECT behaviour, RED on the current code.
#include <gtest/gtest.h>
#include <unistd.h>
#include <memory>
#include <optional>
#include <vector>
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

using ::testing::_;
using ::testing::Return;

class W : public srp::i2c::PCA9685 {
 public:
  using srp::i2c::PCA9685::setI2C;
};

/**
 * F-22 (srednia) mw/i2c_service/controller/pca9685/controller.cpp:86-96,108.
 * channel indexes LED_*_LOOKUP[16] without a range check -> out-of-bounds read for channel >= 16
 * (garbage register addresses are then written to the chip).
 * Expected: SetChannelPosition(channel >= 16) returns an error and writes nothing. Actual: kOk + bogus write.
 */
TEST(AuditBugs, F22_Pca9685ChannelOutOfRangeIsNotRejected) {
  for (uint8_t ch : {16, 17, 200}) {
    W w;
    auto mock = std::make_unique<srp::mock::MockI2CController>();
    EXPECT_CALL(*mock, Write(_, _)).WillRepeatedly(Return(srp::core::ErrorCode::kOk));
    ASSERT_EQ(w.setI2C(std::move(mock)), srp::core::ErrorCode::kOk);
    EXPECT_NE(w.SetChannelPosition(ch, 300), srp::core::ErrorCode::kOk)
        << "F-22: channel " << int(ch) << " accepted, LED_*_LOOKUP[16] read out of bounds (controller.cpp:86-96)";
  }
}

/**
 * F-40 (srednia) mw/i2c_service/controller/pca9685/controller.cpp:107-118 (ComposePosition :100-104).
 * ReadChannelPosition checks only has_value(); a short/empty reply (service returns an empty vector on NACK) makes
 * ComposePosition read val[2], val[3] out of bounds (UB, here a null-data read).
 * Expected: nullopt for a reply shorter than 4 B. Actual: reads out of bounds. Run in a child (death test).
 */
TEST(AuditBugs, F40_Pca9685ReadChannelPositionShortReplyReadsOutOfBounds) {
  auto child = [] {
    W w;
    auto mock = std::make_unique<srp::mock::MockI2CController>();
    EXPECT_CALL(*mock, Read(_, _, _))
        .WillRepeatedly(Return(std::optional<std::vector<uint8_t>>(std::vector<uint8_t>{})));
    if (w.setI2C(std::move(mock)) != srp::core::ErrorCode::kOk) _exit(3);
    auto r = w.ReadChannelPosition(0);
    _exit(r.has_value() ? 1 : 0);
  };
  EXPECT_EXIT(child(), ::testing::ExitedWithCode(0), "")
      << "F-40: ReadChannelPosition on an empty reply reads val[2]/val[3] out of bounds (controller.cpp:100-118)";
}
