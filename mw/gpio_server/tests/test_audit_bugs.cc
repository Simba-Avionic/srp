/**
 * @copyright Copyright (c) 2025
 */
// Audit tests (bugs): assert the CORRECT behaviour, RED on the current code.
#include <memory>
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

using ::testing::_;
using ::testing::Return;

class Wrapper : public srp::mw::GPIOMWService {
 public:
  int TestInit(std::unique_ptr<MockStreamSocket> s, std::unique_ptr<MockGPIO> g) {
    return this->Init(std::move(s), std::move(g));
  }
  void SetConfig(const std::unordered_map<uint8_t, srp::mw::GpioConf>& c) { this->config = c; }
  std::vector<uint8_t> Rx(const std::vector<uint8_t>& d) { return this->RxCallback("", 0, d); }
};

/**
 * F-08 (wysoka) mw/gpio_server/gpio_mw.cpp:59-65 (+ core/common/error_code.h kOk=0).
 * SET for a pin_id missing in the config is answered with {0}, which the client (GPIOController::SetPinValue)
 * reads as kOk. Expected: response[0] != kOk (e.g. kNotDefine). Actual: 0 (= kOk) -> missing igniter/servo-power pin
 * looks like a successful switch.
 */
TEST(AuditBugs, F08_SetUnknownPinIsReportedAsOk) {
  Wrapper w;
  EXPECT_EQ(w.TestInit(std::make_unique<MockStreamSocket>(), std::make_unique<MockGPIO>()), 0);
  w.SetConfig({{1, {21, srp::core::gpio::direction_t::OUT}}});
  srp::mw::gpio::GpioHdr hdr{srp::gpio::ACTION::SET, 99, 1, 0, false};
  auto resp = w.Rx(srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(hdr));
  ASSERT_FALSE(resp.empty());
  EXPECT_NE(resp[0], static_cast<uint8_t>(srp::core::ErrorCode::kOk))
      << "F-08: SET of unknown pin_id 99 answered {0}==kOk (gpio_mw.cpp:59-65)";
}

/**
 * F-08b: same for a frame that cannot be parsed (gpio_mw.cpp:52-55 returns {0}).
 */
TEST(AuditBugs, F08_UnparsableFrameIsReportedAsOk) {
  Wrapper w;
  EXPECT_EQ(w.TestInit(std::make_unique<MockStreamSocket>(), std::make_unique<MockGPIO>()), 0);
  w.SetConfig({{1, {21, srp::core::gpio::direction_t::OUT}}});
  auto resp = w.Rx({0xFF});
  ASSERT_FALSE(resp.empty());
  EXPECT_NE(resp[0], static_cast<uint8_t>(srp::core::ErrorCode::kOk))
      << "F-08: unparsable GPIO frame answered {0}==kOk (gpio_mw.cpp:52-55)";
}

/**
 * F-12 (srednia) mw/gpio_server/gpio_mw_did.h:55 — payload[0], payload[1] are read in LogInfo BEFORE the size
 * check at :57. Expected: Write({}) returns an error without touching payload[0]. Actual: out-of-bounds read
 * (UB). The target is built with -D_GLIBCXX_ASSERTIONS so the OOB operator[] aborts deterministically.
 */
TEST(AuditBugs, F12_DidWriteEmptyPayloadReadsOutOfBounds) {
  auto drv = std::make_shared<MockGPIO>();
  srp::mw::GpioMWDID did(ara::core::InstanceSpecifier("audit/gpio"), drv, {});
  EXPECT_EXIT(
      {
        auto r = did.Write(std::vector<uint8_t>{});
        _exit(r.HasValue() ? 1 : 0);  // error code (HasValue()==false) expected
      },
      ::testing::ExitedWithCode(0), "")
      << "F-12: Write(empty payload) reads payload[0]/[1] before the size check (gpio_mw_did.h:55)";
}

/**
 * F-14 (niska) mw/gpio_server/gpio_mw.cpp:98-120. SET value=0 returns before pin_expire is cleaned up, so the
 * stale infinite_active=true (from SET(p,1,0)) survives and a later SET(p,1,200) is never switched off.
 * Expected: after SET(p,1,0); SET(p,0); SET(p,1,200) the pin has a finite expiry (infinite_active==false).
 * Actual: infinite_active stays true.
 */
TEST(AuditBugs, F14_StaleInfiniteActiveAfterSetZero) {
  Wrapper w;
  auto drv = std::make_unique<MockGPIO>();
  EXPECT_CALL(*drv, setValue(_, _, _)).WillRepeatedly(Return(srp::core::ErrorCode::kOk));
  EXPECT_EQ(w.TestInit(std::make_unique<MockStreamSocket>(), std::move(drv)), 0);
  w.SetConfig({{1, {21, srp::core::gpio::direction_t::OUT}}});
  auto send = [&](uint8_t v, uint16_t ms) {
    srp::mw::gpio::GpioHdr h{srp::gpio::ACTION::SET, 1, v, ms, false};
    return w.Rx(srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(h));
  };
  send(1, 0);
  send(0, 0);
  send(1, 200);
  ASSERT_TRUE(w.pin_expire.count(1)) << "harness: no pin_expire entry";
  EXPECT_FALSE(w.pin_expire.at(1).infinite_active)
      << "F-14: pin stays infinite_active after SET(1,0) -> SET(0) -> SET(1,200ms) (gpio_mw.cpp:98-120)";
}
