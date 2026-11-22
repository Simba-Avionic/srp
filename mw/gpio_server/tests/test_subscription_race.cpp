/**
 * @file test_subscription_race.cpp
 * @brief Unit test: subscriptions changed by the RX thread while the polling loop iterates them (F-10)
 *
 * @copyright Copyright (c) 2025
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
using ::testing::Return;

class TestService : public srp::mw::GPIOMWService {
 public:
  void TestInit(std::unique_ptr<MockStreamSocket> socket, std::unique_ptr<MockGPIO> gpio) {
    this->Init(std::move(socket), std::move(gpio));
  }
  void SetConfig(const std::unordered_map<uint8_t, srp::mw::GpioConf>& conf) { this->config = conf; }
  std::vector<uint8_t> Send(srp::gpio::ACTION action, uint8_t pin, uint8_t value) {
    srp::mw::gpio::GpioHdr hdr{action, pin, value};
    return this->RxCallback("", 0, srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(hdr));
  }
  void Poll(const std::stop_token& token) { this->PollSubscribedPinsLoop(token); }
};

std::unordered_map<uint8_t, srp::mw::GpioConf> Config() {
  return {{1, {21, srp::core::gpio::direction_t::IN}},
          {2, {22, srp::core::gpio::direction_t::IN}},
          {3, {23, srp::core::gpio::direction_t::IN}}};
}
}  // namespace

TEST(GpioSubscriptionRace, ChangeOfASubscribedPinIsSentToItsController) {
  TestService service;
  auto socket = std::make_unique<MockStreamSocket>();
  auto gpio = std::make_unique<MockGPIO>();
  std::atomic<int> reads{0};
  std::atomic<int> sent{0};
  // the pin reads 0 first, then 1
  ON_CALL(*gpio, getValue(_, _)).WillByDefault(Invoke([&](const uint16_t&, bool) -> uint8_t {
    return reads++ < 2 ? 0 : 1;
  }));
  EXPECT_CALL(*socket, Transmit("SRP.GPIO.1", _, _)).WillRepeatedly(Invoke(
      [&](const std::string&, const std::uint16_t, std::vector<std::uint8_t>) {
        ++sent;
        return std::optional<std::vector<uint8_t>>{std::vector<uint8_t>{}};
      }));
  service.TestInit(std::move(socket), std::move(gpio));
  service.SetConfig(Config());
  auto res = service.Send(srp::gpio::ACTION::SUBSCRIBE, 1, 0);  // controller ID 1
  ASSERT_EQ(res.size(), 1U);
  ASSERT_EQ(res[0], 1);
  {
    std::jthread poll([&](const std::stop_token& t) { service.Poll(t); });
    for (int i = 0; i < 100 && sent.load() == 0; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
  EXPECT_GE(sent.load(), 1);
}

TEST(GpioSubscriptionRace, SubscribingWhilePollingDoesNotCorruptTheContainers) {
  TestService service;
  auto socket = std::make_unique<MockStreamSocket>();
  auto gpio = std::make_unique<MockGPIO>();
  std::atomic<int> reads{0};
  // every read differs from the previous one -> every polling pass walks callbacks and transmits
  ON_CALL(*gpio, getValue(_, _)).WillByDefault(Invoke([&](const uint16_t&, bool) -> uint8_t {
    return static_cast<uint8_t>(reads++ & 1);
  }));
  ON_CALL(*socket, Transmit(_, _, _))
      .WillByDefault(Return(std::optional<std::vector<uint8_t>>{std::vector<uint8_t>{}}));
  service.TestInit(std::move(socket), std::move(gpio));
  service.SetConfig(Config());
  {
    std::jthread poll([&](const std::stop_token& t) { service.Poll(t); });
    for (int i = 0; i < 3000; ++i) {
      const uint8_t pin = static_cast<uint8_t>(1 + (i % 3));
      auto id = service.Send(srp::gpio::ACTION::SUBSCRIBE, pin, 0);
      ASSERT_EQ(id.size(), 1U);
      service.Send(srp::gpio::ACTION::UNSUBSCRIBE, pin, id[0]);
    }
  }
  SUCCEED();
}
