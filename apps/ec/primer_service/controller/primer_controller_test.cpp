/**
 * @copyright Copyright (c) 2026
 */
// F-05: an ADC read error must not leave the primer CONNECTED (EnablePrimer would fire on a stale state).
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "apps/ec/primer_service/controller/primer_controller.hpp"
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"

using ::testing::_;
using ::testing::Return;

namespace srp {
namespace primer {
namespace {
using Reply = std::optional<std::vector<uint8_t>>;
const Reply kConnected = std::vector<uint8_t>{0x0E, 0x8C};     // 3724 -> 3.0 V
const Reply kNotConnected = std::vector<uint8_t>{0x00, 0x00};  // 0 V
const Reply kError = std::nullopt;                              // I2C/ADC error

class PrimerControllerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    auto mock = std::make_unique<mock::MockI2CController>();
    i2c_ = mock.get();
    ::testing::Mock::AllowLeak(i2c_);  // the controller is leaked on purpose (see below)
    // Intentionally leaked: ~GPIOController -> StreamIpcSocket::StopRXThread() dereferences rx_thred, which is null
    // when the socket never listened (separate latent bug, out of scope for F-05).
    ctrl_ = new PrimerController();
    ctrl_->Initialize(std::move(mock));
  }
  void Read(const Reply& reply) {
    EXPECT_CALL(*i2c_, WriteRead(_, _, _)).WillOnce(Return(reply));
    ctrl_->VerifyPrimerConection();
  }
  mock::MockI2CController* i2c_ = nullptr;
  PrimerController* ctrl_ = nullptr;
};

TEST_F(PrimerControllerTest, SingleReadErrorKeepsState) {
  Read(kConnected);
  ASSERT_EQ(ctrl_->GetPrimerState(), PrimerState_t::kCONNECTED);
  Read(kError);
  EXPECT_EQ(ctrl_->GetPrimerState(), PrimerState_t::kCONNECTED);
  Read(kError);
  EXPECT_EQ(ctrl_->GetPrimerState(), PrimerState_t::kCONNECTED);
}

TEST_F(PrimerControllerTest, ThreeConsecutiveReadErrorsGiveUnknown) {
  Read(kConnected);
  Read(kError);
  Read(kError);
  Read(kError);
  EXPECT_EQ(ctrl_->GetPrimerState(), PrimerState_t::kUNKNOWN);
  EXPECT_FALSE(ctrl_->EnablePrimer()) << "F-05: EnablePrimer must refuse in UNKNOWN";
}

TEST_F(PrimerControllerTest, GoodReadResetsErrorCounter) {
  Read(kConnected);
  Read(kError);
  Read(kError);
  Read(kConnected);  // resets the counter
  Read(kError);
  Read(kError);
  EXPECT_EQ(ctrl_->GetPrimerState(), PrimerState_t::kCONNECTED) << "2+2 errors separated by a good read";
  Read(kError);
  EXPECT_EQ(ctrl_->GetPrimerState(), PrimerState_t::kUNKNOWN);
}

TEST_F(PrimerControllerTest, RecoversFromUnknown) {
  Read(kError);
  Read(kError);
  Read(kError);
  ASSERT_EQ(ctrl_->GetPrimerState(), PrimerState_t::kUNKNOWN);
  Read(kNotConnected);
  EXPECT_EQ(ctrl_->GetPrimerState(), PrimerState_t::kNOT_CONNECTED);
}

}  // namespace
}  // namespace primer
}  // namespace srp
