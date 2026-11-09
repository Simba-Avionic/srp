/**
 * @file test_eeprom_writebyte.cc
 * @brief WriteByte uzywa PageWrite (jeden write() z adresem i danymi) (F-41)
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <memory>
#include <optional>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "mw/i2c_service/controller/24lc32at/controller.hpp"
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

using ::testing::_;
using ::testing::Return;

TEST(EepromWriteByteTest, WriteByteSendsAddressAndDataAsSinglePageWrite) {
  auto i2c = std::make_unique<srp::mock::MockI2CController>();
  auto* mock = i2c.get();
  // Init: potwierdzenie ukladu (ReadByte) i ACK polling
  EXPECT_CALL(*mock, WriteReadBuffer(_, _, _))
      .WillRepeatedly(Return(std::optional<std::vector<uint8_t>>(std::vector<uint8_t>{0x5A})));
  EXPECT_CALL(*mock, Write(_, _)).WillRepeatedly(Return(srp::core::ErrorCode::kOk));
  srp::i2c::EEPROM24LC32AT eeprom;
  ASSERT_EQ(eeprom.Init(std::move(i2c)), srp::core::ErrorCode::kOk);

  EXPECT_CALL(*mock, PageWrite(0x50, std::vector<uint8_t>{0x01, 0x23, 0x5A}))
      .WillOnce(Return(srp::core::ErrorCode::kOk));
  // 3-bajtowy Write() jest odrzucany przez I2CDriver - nie wolno go uzywac
  EXPECT_CALL(*mock, Write(_, std::vector<uint8_t>{0x01, 0x23, 0x5A})).Times(0);
  EXPECT_EQ(eeprom.WriteByte(0x0123, 0x5A), srp::core::ErrorCode::kOk);
}

TEST(EepromWriteByteTest, PageWriteFailureIsReported) {
  auto i2c = std::make_unique<srp::mock::MockI2CController>();
  auto* mock = i2c.get();
  EXPECT_CALL(*mock, WriteReadBuffer(_, _, _))
      .WillRepeatedly(Return(std::optional<std::vector<uint8_t>>(std::vector<uint8_t>{0x00})));
  EXPECT_CALL(*mock, Write(_, _)).WillRepeatedly(Return(srp::core::ErrorCode::kOk));
  srp::i2c::EEPROM24LC32AT eeprom;
  ASSERT_EQ(eeprom.Init(std::move(i2c)), srp::core::ErrorCode::kOk);
  EXPECT_CALL(*mock, PageWrite(_, _)).WillOnce(Return(srp::core::ErrorCode::kConnectionError));
  EXPECT_EQ(eeprom.WriteByte(0x0000, 0x01), srp::core::ErrorCode::kConnectionError);
}
