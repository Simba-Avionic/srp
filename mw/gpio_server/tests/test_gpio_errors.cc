/**
 * @copyright Copyright (c) 2026
 */
// F-08: the GPIO service must not answer kOk (0) for an unknown pin or an unparsable frame.
#include <memory>
#include <utility>
#include <vector>
#include "gtest/gtest.h"
#define private public
#define protected public
#include "mw/gpio_server/gpio_mw.hpp"
#undef protected
#undef private
#include "mw/gpio_server/data/header.hpp"
#include "core/gpio/mock/mock_gpio_driver.h"
#include "communication-core/sockets/mock/mockSocket.h"

namespace {
class Wrapper : public srp::mw::GPIOMWService {
 public:
  int TestInit(std::unique_ptr<MockStreamSocket> s, std::unique_ptr<MockGPIO> g) {
    return this->Init(std::move(s), std::move(g));
  }
  void SetConfig(const std::unordered_map<uint8_t, srp::mw::GpioConf>& c) { this->config = c; }
  std::vector<uint8_t> Rx(const std::vector<uint8_t>& d) { return this->RxCallback("", 0, d); }
};

std::vector<uint8_t> Frame(srp::gpio::ACTION action, uint8_t pin, uint8_t value = 1) {
  srp::mw::gpio::GpioHdr hdr{action, pin, value, 0, false};
  return srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(hdr);
}

std::unique_ptr<Wrapper> MakeService() {
  auto w = std::make_unique<Wrapper>();
  EXPECT_EQ(w->TestInit(std::make_unique<MockStreamSocket>(), std::make_unique<MockGPIO>()), 0);
  w->SetConfig({{1, {21, srp::core::gpio::direction_t::OUT}}});
  return w;
}
}  // namespace

TEST(GpioErrors, SetUnknownPinIsNotOk) {
  auto w = MakeService();
  auto resp = w->Rx(Frame(srp::gpio::ACTION::SET, 99));
  ASSERT_FALSE(resp.empty());
  EXPECT_EQ(resp[0], static_cast<uint8_t>(srp::core::ErrorCode::kNotDefine));
}

TEST(GpioErrors, GetUnknownPinIsNotAValidResponse) {
  auto w = MakeService();
  auto resp = w->Rx(Frame(srp::gpio::ACTION::GET, 99));
  ASSERT_EQ(resp.size(), 1u);
  EXPECT_NE(resp[0], static_cast<uint8_t>(srp::core::ErrorCode::kOk));
}

TEST(GpioErrors, UnparsableFrameIsNotOk) {
  auto w = MakeService();
  auto resp = w->Rx({0xFF});
  ASSERT_FALSE(resp.empty());
  EXPECT_NE(resp[0], static_cast<uint8_t>(srp::core::ErrorCode::kOk));
}

TEST(GpioErrors, SubscribeUnknownPinKeepsZeroConvention) {
  // for SUBSCRIBE any non-zero byte is a controller ID, so the failure answer must stay 0
  auto w = MakeService();
  auto resp = w->Rx(Frame(srp::gpio::ACTION::SUBSCRIBE, 99, 0));
  ASSERT_EQ(resp.size(), 1u);
  EXPECT_EQ(resp[0], 0);
  resp = w->Rx(Frame(srp::gpio::ACTION::UNSUBSCRIBE, 99, 5));
  ASSERT_EQ(resp.size(), 1u);
  EXPECT_EQ(resp[0], 0);
}

TEST(GpioErrors, SetKnownPinStillOk) {
  auto w = MakeService();
  auto resp = w->Rx(Frame(srp::gpio::ACTION::SET, 1, 0));
  ASSERT_FALSE(resp.empty());
  EXPECT_EQ(resp[0], static_cast<uint8_t>(srp::core::ErrorCode::kOk));
}
