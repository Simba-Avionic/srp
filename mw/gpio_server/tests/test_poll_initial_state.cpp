/**
 * @file test_poll_initial_state.cpp
 * @brief Unit test: the polling loop starts from the real pin state, not from 0 (F-11)
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

TEST(GpioPollInitialState, PinHighAtStartIsNotReportedAsAChange) {
  Wrapper service;
  auto socket = std::make_unique<MockStreamSocket>();
  auto gpio = std::make_unique<MockGPIO>();
  ON_CALL(*gpio, getValue(_, _)).WillByDefault(::testing::Return(1));
  // F-11: no callback may be sent for a pin that was high from the start
  EXPECT_CALL(*socket, Transmit(_, _, _)).Times(0);
  service.TestInit(std::move(socket), std::move(gpio));
  service.SetConfig({{1, {21, srp::core::gpio::direction_t::IN}}});
  auto res = service.Rx(srp::gpio::ACTION::SUBSCRIBE, 1, 0);
  ASSERT_EQ(res.size(), 1U);
  {
    std::jthread poll([&](const std::stop_token& t) { service.Poll(t); });
    std::this_thread::sleep_for(std::chrono::milliseconds(450));
  }
}

TEST(GpioPollInitialState, RealChangeAfterStartIsReported) {
  Wrapper service;
  auto socket = std::make_unique<MockStreamSocket>();
  auto gpio = std::make_unique<MockGPIO>();
  std::atomic<int> reads{0};
  // the first read (initial state) is 0, the next ones are 1
  ON_CALL(*gpio, getValue(_, _)).WillByDefault(Invoke([&](const uint16_t&, bool) -> uint8_t {
    return reads++ == 0 ? 0 : 1;
  }));
  std::atomic<int> sent{0};
  EXPECT_CALL(*socket, Transmit(_, _, _)).WillRepeatedly(Invoke(
      [&](const std::string&, const std::uint16_t, std::vector<std::uint8_t>) {
        ++sent;
        return std::optional<std::vector<uint8_t>>{std::vector<uint8_t>{}};
      }));
  service.TestInit(std::move(socket), std::move(gpio));
  service.SetConfig({{1, {21, srp::core::gpio::direction_t::IN}}});
  service.Rx(srp::gpio::ACTION::SUBSCRIBE, 1, 0);
  {
    std::jthread poll([&](const std::stop_token& t) { service.Poll(t); });
    for (int i = 0; i < 60 && sent.load() == 0; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
  EXPECT_EQ(sent.load(), 1);
}
