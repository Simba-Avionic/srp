/**
 * @file test_ina219_init.cpp
 * @brief Unit test: INA219::Initialize writes the 3-byte register frames with PageWrite (F-28)
 *
 * @copyright Copyright (c) 2026
 */
#include <memory>
#include <optional>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define private public  // NOLINT
#include "mw/i2c_service/controller/ina219/controller.hpp"
#undef private
#include "communication-core/sockets/mock/mockSocket.h"

using ::testing::_;
using ::testing::Invoke;

namespace {
constexpr uint8_t kPageWrite = 0x05;
constexpr uint8_t kAddress = 0x40;
}  // namespace

TEST(Ina219Init, ConfigAndCalibrationAreSentAsPageWrite) {
  srp::i2c::INA219 ina;
  auto socket = std::make_unique<MockStreamSocket>();
  std::vector<std::vector<uint8_t>> frames;
  EXPECT_CALL(*socket, Transmit(_, _, _)).WillRepeatedly(Invoke(
      [&](const std::string&, const std::uint16_t, std::vector<std::uint8_t> frame) {
        frames.push_back(frame);
        return std::optional<std::vector<uint8_t>>{std::vector<uint8_t>{1}};  // chip ACK
      }));
  ASSERT_EQ(ina.i2c_.Init(std::move(socket)), srp::core::ErrorCode::kOk);
  EXPECT_EQ(ina.Initialize(kAddress), srp::core::ErrorCode::kOk);
  ASSERT_EQ(frames.size(), 2U);
  // frame: action, address, payload size, payload...
  for (const auto& f : frames) {
    ASSERT_GE(f.size(), 6U);
    EXPECT_EQ(f[0], kPageWrite) << "F-28: the 3-byte register frame is not sent as PageWrite";
    EXPECT_EQ(f[1], kAddress);
  }
  EXPECT_EQ(std::vector<uint8_t>(frames[0].begin() + 3, frames[0].end()), (std::vector<uint8_t>{0x00, 0x0C, 0x47}));
  EXPECT_EQ(std::vector<uint8_t>(frames[1].begin() + 3, frames[1].end()), (std::vector<uint8_t>{0x05, 0x41, 0x89}));
}

TEST(Ina219Init, NackOnConfigurationFailsTheInit) {
  srp::i2c::INA219 ina;
  auto socket = std::make_unique<MockStreamSocket>();
  EXPECT_CALL(*socket, Transmit(_, _, _)).WillOnce(Invoke(
      [&](const std::string&, const std::uint16_t, std::vector<std::uint8_t>) {
        return std::optional<std::vector<uint8_t>>{std::vector<uint8_t>{}};  // empty answer = chip NACK
      }));
  ASSERT_EQ(ina.i2c_.Init(std::move(socket)), srp::core::ErrorCode::kOk);
  EXPECT_EQ(ina.Initialize(kAddress), srp::core::ErrorCode::kConnectionError);
}
