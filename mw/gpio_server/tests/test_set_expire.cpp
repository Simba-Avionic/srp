/**
 * @file test_set_expire.cpp
 * @brief Unit test: SET value 0 clears the pending auto-disable of the pin (F-14)
 *
 * @copyright Copyright (c) 2026
 */
#include <atomic>
#include <chrono>  // NOLINT
#include <memory>
#include <thread>  // NOLINT
#include <unordered_map>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "mw/gpio_server/data/header.hpp"
#include "mw/gpio_server/gpio_mw.hpp"
#include "core/gpio/mock/mock_gpio_driver.h"
#include "communication-core/sockets/mock/mockSocket.h"

namespace {
using ::testing::_;
using ::testing::Invoke;

class Wrapper : public srp::mw::GPIOMWService {
 public:
  void TestInit(std::unique_ptr<MockStreamSocket> s, std::unique_ptr<MockGPIO> g) {
    this->Init(std::move(s), std::move(g));
  }
  void SetConfig(const std::unordered_map<uint8_t, srp::mw::GpioConf>& c) { this->config = c; }
  std::vector<uint8_t> Rx(srp::gpio::ACTION a, uint8_t pin, uint8_t value, uint16_t period = 0) {
    srp::mw::gpio::GpioHdr hdr{a, pin, value, period, false};
    return this->RxCallback("", 0, srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(hdr));
  }
  void Poll(const std::stop_token& t) { this->PollSubscribedPinsLoop(t); }
  std::chrono::milliseconds Expire() { return this->CheckPinsExpired(); }
};
}  // namespace

TEST(GpioSetExpire, TimedSetAfterInfiniteSetAndOffExpires) {
  Wrapper service;
  auto socket = std::make_unique<MockStreamSocket>();
  auto gpio = std::make_unique<MockGPIO>();
  std::vector<int> values;
  ON_CALL(*gpio, setValue(_, _, _)).WillByDefault(Invoke(
      [&](const uint16_t&, const uint8_t& v, bool) { values.push_back(v); return srp::core::ErrorCode::kOk; }));
  service.TestInit(std::move(socket), std::move(gpio));
  service.SetConfig({{1, {21, srp::core::gpio::direction_t::OUT}}});

  service.Rx(srp::gpio::ACTION::SET, 1, 1, 0);    // on, no time limit (infinite)
  service.Rx(srp::gpio::ACTION::SET, 1, 0, 0);    // off
  service.Rx(srp::gpio::ACTION::SET, 1, 1, 200);  // on for 200 ms
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  service.Expire();
  // 1, 0, 1 and then the auto-disable (0)
  ASSERT_EQ(values.size(), 4U) << "F-14: the timed SET did not expire";
  EXPECT_EQ(values.back(), 0);
}

TEST(GpioSetExpire, InfiniteSetStaysOn) {
  Wrapper service;
  auto socket = std::make_unique<MockStreamSocket>();
  auto gpio = std::make_unique<MockGPIO>();
  std::vector<int> values;
  ON_CALL(*gpio, setValue(_, _, _)).WillByDefault(Invoke(
      [&](const uint16_t&, const uint8_t& v, bool) { values.push_back(v); return srp::core::ErrorCode::kOk; }));
  service.TestInit(std::move(socket), std::move(gpio));
  service.SetConfig({{1, {21, srp::core::gpio::direction_t::OUT}}});
  service.Rx(srp::gpio::ACTION::SET, 1, 1, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  service.Expire();
  EXPECT_EQ(values.size(), 1U);
}
