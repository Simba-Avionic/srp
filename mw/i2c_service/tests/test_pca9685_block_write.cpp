/**
 * @file test_pca9685_block_write.cpp
 * @brief Unit test: PCA9685 writes a channel in ONE I2C transaction with register auto-increment (F-32)
 *
 * @copyright Copyright (c) 2026
 */
#include <memory>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ElementsAre;

namespace {
constexpr uint8_t kAddr = 0x70;
}

TEST(Pca9685BlockWrite, InitSetsAutoIncrementInMode1) {
  auto i2c = std::make_unique<srp::mock::MockI2CController>();
  auto* m = i2c.get();
  EXPECT_CALL(*m, Init(_)).WillOnce(Return(srp::core::ErrorCode::kOk));
  {
    InSequence seq;
    EXPECT_CALL(*m, Write(kAddr, std::vector<uint8_t>{0x00, 0x31})).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*m, Write(kAddr, std::vector<uint8_t>{0xFE, 0x79})).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*m, Write(kAddr, std::vector<uint8_t>{0x00, 0xA1})).WillOnce(Return(srp::core::ErrorCode::kOk));
  }
  srp::i2c::PCA9685 pca;
  EXPECT_EQ(pca.Init(std::move(i2c)), srp::core::ErrorCode::kOk);
}

class Pca9685BlockTest : public ::testing::Test {
 protected:
  Pca9685BlockTest() {
    auto i2c = std::make_unique<srp::mock::MockI2CController>();
    m_ = i2c.get();
    EXPECT_CALL(*m_, Init(_)).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*m_, Write(_, _)).WillRepeatedly(Return(srp::core::ErrorCode::kOk));
    EXPECT_EQ(pca_.Init(std::move(i2c)), srp::core::ErrorCode::kOk);
  }
  srp::i2c::PCA9685 pca_;
  srp::mock::MockI2CController* m_;
};

TEST_F(Pca9685BlockTest, ChannelIsWrittenInOneTransactionInRegisterOrder) {
  // channel 3: LED3_ON_L = 0x12; data: ON_L=0, ON_H=0, OFF_L=0x66, OFF_H=0x01 (pos 0x166 = 358)
  EXPECT_CALL(*m_, Write(_, _)).Times(0);
  EXPECT_CALL(*m_, PageWrite(kAddr, std::vector<uint8_t>{0x12, 0x00, 0x00, 0x66, 0x01}))
      .Times(1).WillOnce(Return(srp::core::ErrorCode::kOk));
  EXPECT_EQ(pca_.SetChannelPosition(3, 0x166), srp::core::ErrorCode::kOk);
}

TEST_F(Pca9685BlockTest, FirstAndLastChannelUseTheirOwnRegisters) {
  EXPECT_CALL(*m_, PageWrite(kAddr, std::vector<uint8_t>{0x06, 0x00, 0x00, 0x66, 0x01}))
      .WillOnce(Return(srp::core::ErrorCode::kOk));
  EXPECT_CALL(*m_, PageWrite(kAddr, std::vector<uint8_t>{0x42, 0x00, 0x00, 0x00, 0x02}))
      .WillOnce(Return(srp::core::ErrorCode::kOk));
  EXPECT_EQ(pca_.SetChannelPosition(0, 0x166), srp::core::ErrorCode::kOk);
  EXPECT_EQ(pca_.SetChannelPosition(15, 512), srp::core::ErrorCode::kOk);
}

TEST_F(Pca9685BlockTest, TransactionErrorIsReturnedAndNothingElseIsWritten) {
  EXPECT_CALL(*m_, Write(_, _)).Times(0);
  EXPECT_CALL(*m_, PageWrite(_, _)).Times(1).WillOnce(Return(srp::core::ErrorCode::kConnectionError));
  EXPECT_EQ(pca_.SetChannelPosition(3, 100), srp::core::ErrorCode::kConnectionError);
}
