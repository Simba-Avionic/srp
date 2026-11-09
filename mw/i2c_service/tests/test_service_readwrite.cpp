/**
 * @file test_service_readwrite.cpp
 * @brief Unit test: I2CService::ReadWrite validates the payload size (F-31)
 *
 * @copyright Copyright (c) 2026
 */
#include <memory>
#include <optional>
#include <vector>

#include "gtest/gtest.h"

// white-box access: I2CService is final and its ReadWrite/Init are protected
#define private public  // NOLINT
#define protected public  // NOLINT
#include "mw/i2c_service/service/i2c_service.h"
#undef protected
#undef private
#include "communication-core/sockets/mock/mockSocket.h"

namespace {
class FakeDriver : public srp::core::i2c::II2CDriver {
 public:
  int read_write_calls = 0;
  uint8_t last_reg = 0;
  uint8_t last_size = 0;
  srp::core::ErrorCode Init() override { return srp::core::ErrorCode::kOk; }
  srp::core::ErrorCode Ioctl(const uint8_t, const uint16_t) override { return srp::core::ErrorCode::kOk; }
  srp::core::ErrorCode Write(const std::vector<uint8_t>&) override { return srp::core::ErrorCode::kOk; }
  srp::core::ErrorCode Write(const uint8_t&) override { return srp::core::ErrorCode::kOk; }
  std::optional<std::vector<uint8_t>> Read(const uint8_t) override { return std::nullopt; }
  srp::core::ErrorCode PageWrite(std::vector<uint8_t>) override { return srp::core::ErrorCode::kOk; }
  std::optional<std::vector<uint8_t>> ReadWrite(const uint8_t& reg, const uint8_t size) override {
    ++read_write_calls;
    last_reg = reg;
    last_size = size;
    return std::vector<uint8_t>(size, 0xAB);
  }
  std::optional<std::vector<uint8_t>> ReadWrite(const std::vector<uint8_t>&, const uint8_t) override {
    return std::nullopt;
  }
};

class Wrapper {
 public:
  srp::mw::I2CService service;
  void TestInit(std::shared_ptr<srp::core::i2c::II2CDriver> d) {
    service.Init(std::move(d), std::make_unique<MockStreamSocket>());
  }
  std::optional<std::vector<uint8_t>> Rw(const std::vector<uint8_t>& payload) {
    return service.ReadWrite(payload, nullptr);
  }
};
}  // namespace

TEST(I2cServiceReadWrite, EmptyPayloadIsRejectedWithoutTouchingTheDriver) {
  auto drv = std::make_shared<FakeDriver>();
  Wrapper w;
  w.TestInit(drv);
  EXPECT_FALSE(w.Rw({}).has_value()) << "F-31: an empty payload was accepted";
  EXPECT_EQ(drv->read_write_calls, 0);
}

TEST(I2cServiceReadWrite, WrongSizesAreRejected) {
  auto drv = std::make_shared<FakeDriver>();
  Wrapper w;
  w.TestInit(drv);
  EXPECT_FALSE(w.Rw({0x10}).has_value());
  EXPECT_FALSE(w.Rw({0x10, 4, 0x20}).has_value());
  EXPECT_FALSE(w.Rw({0x10, 4, 0x20, 4}).has_value());
  EXPECT_EQ(drv->read_write_calls, 0);
}

TEST(I2cServiceReadWrite, RegisterAndSizeAreReadFromTheTwoBytes) {
  auto drv = std::make_shared<FakeDriver>();
  Wrapper w;
  w.TestInit(drv);
  auto res = w.Rw({0x10, 4});
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(res->size(), 4U);
  EXPECT_EQ(drv->last_reg, 0x10);
  EXPECT_EQ(drv->last_size, 4);
}
