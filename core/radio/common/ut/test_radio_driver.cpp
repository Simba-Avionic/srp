/**
 * Copyright (c) 2024
 *
 * @file test_radio_driver.cpp
 * @brief RadioDriver + FakeModem: what the platform contract suite does not cover - Stop() during
 * Transmit(), modem errors (OnError(fatal) exactly once), link statistics from the modem, MaxFrameSize,
 * the one-frame TX queue, Configure() errors and the threading rules of the modem.
 * Every test runs for a half- and a full-duplex modem.
 */
#include <gtest/gtest.h>

#include <chrono>  // NOLINT
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <tuple>
#include <vector>

#include "ara/com/mavlink/testing/radio_driver_contract_test.h"
#include "ara/com/mavlink/transport_error.h"
#include "core/radio/common/radio_driver.hpp"
#include "core/radio/common/testing/fake_modem.hpp"

namespace {

using ara::com::mavlink::IsTransportError;
using ara::com::mavlink::MakeTransportError;
using ara::com::mavlink::RadioLinkStats;
using ara::com::mavlink::TransportErrc;
using ara::com::mavlink::testing::RecordingListener;
using srp::core::radio::RadioCaps;
using srp::core::radio::RadioDriver;
using srp::core::radio::testing::FakeModem;
using srp::core::radio::testing::FakeModemOptions;

bool WaitFor(const std::function<bool()>& pred, int timeout_ms) {
  const auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
  while (std::chrono::steady_clock::now() < end) {
    if (pred()) {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
  return pred();
}

std::vector<std::uint8_t> Pattern(std::size_t n, std::uint8_t seed = 0) {
  std::vector<std::uint8_t> v(n);
  for (std::size_t i = 0; i < n; ++i) {
    v[i] = static_cast<std::uint8_t>(seed + i);
  }
  return v;
}

class RadioDriverTest : public ::testing::TestWithParam<bool> {
 protected:
  void SetUp() override { Make(255); }

  void Make(std::size_t mtu) {
    FakeModemOptions opt;
    opt.caps = RadioCaps{mtu, false, GetParam()};
    modem_ = std::make_shared<FakeModem>(opt);
    driver_ = std::make_shared<RadioDriver>("test", modem_);
  }
  void TearDown() override { driver_->Stop(); }

  std::shared_ptr<FakeModem> modem_;
  std::shared_ptr<RadioDriver> driver_;
};

TEST_P(RadioDriverTest, StopDuringTransmitGivesZeroCallbacks) {
  RecordingListener l;
  ASSERT_TRUE(driver_->Start(l, {}).HasValue());
  modem_->HoldTransmit(true);  // Transmit() blocks "on the air"
  ASSERT_TRUE(driver_->Send(Pattern(100)).HasValue());
  ASSERT_TRUE(WaitFor([&] { return modem_->transmit_entered() >= 1; }, 3000));
  const auto t0 = std::chrono::steady_clock::now();
  driver_->Stop();
  EXPECT_LT(std::chrono::steady_clock::now() - t0, std::chrono::seconds(2)) << "Stop() hung in Transmit()";
  l.Seal();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_EQ(l.violations(), 0);
  EXPECT_EQ(l.total(), 0) << "a callback although the frame never left and nothing was received";
  EXPECT_EQ(modem_->transmitted_frames(), 0);
}

TEST_P(RadioDriverTest, TransmitErrorGivesOneFatalError) {
  RecordingListener l;
  ASSERT_TRUE(driver_->Start(l, {}).HasValue());
  modem_->FailTransmit(MakeTransportError(TransportErrc::kPersistent, "modem lost"));
  ASSERT_TRUE(driver_->Send(Pattern(50)).HasValue());
  ASSERT_TRUE(WaitFor([&] { return l.fatal_errors() >= 1; }, 3000));
  // the radio is unusable until Stop()/Start(): Send() fails (and is not kBusy), no second OnError
  const auto r = driver_->Send(Pattern(50));
  ASSERT_FALSE(r.HasValue());
  EXPECT_FALSE(IsTransportError(r.Error(), TransportErrc::kBusy));
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_EQ(l.fatal_errors(), 1);
  EXPECT_EQ(l.errors(), 1);

  // the daemon restarts the link: Stop() + Start() work again
  driver_->Stop();
  modem_->FailTransmit(std::nullopt);
  RecordingListener l2;
  ASSERT_TRUE(driver_->Start(l2, {}).HasValue());
  const auto frame = Pattern(30, 4);
  ASSERT_TRUE(driver_->Send(frame).HasValue());
  EXPECT_EQ(modem_->PeerRead(frame.size(), 3000), frame);
  EXPECT_EQ(l2.errors(), 0);
}

TEST_P(RadioDriverTest, ReceiveErrorGivesOneFatalError) {
  RecordingListener l;
  ASSERT_TRUE(driver_->Start(l, {}).HasValue());
  modem_->FailReceive(MakeTransportError(TransportErrc::kPersistent, "modem lost"));
  ASSERT_TRUE(WaitFor([&] { return l.fatal_errors() >= 1; }, 3000));
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_EQ(l.fatal_errors(), 1);
}

TEST_P(RadioDriverTest, ErrorsOnBothDirectionsStillGiveOneFatalError) {
  RecordingListener l;
  ASSERT_TRUE(driver_->Start(l, {}).HasValue());
  modem_->FailTransmit(MakeTransportError(TransportErrc::kPersistent, "tx lost"));
  modem_->FailReceive(MakeTransportError(TransportErrc::kPersistent, "rx lost"));
  std::ignore = driver_->Send(Pattern(10));  // accepted or already refused: both are fine
  ASSERT_TRUE(WaitFor([&] { return l.fatal_errors() >= 1; }, 3000));
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_EQ(l.fatal_errors(), 1);
  EXPECT_EQ(l.errors(), 1);
}

TEST_P(RadioDriverTest, StatsFromModemReachListener) {
  RecordingListener l;
  ASSERT_TRUE(driver_->Start(l, {}).HasValue());
  RadioLinkStats a;
  a.valid = RadioLinkStats::kRssi | RadioLinkStats::kNoiseFloor;
  a.rssi_dbm = -73;
  a.noise_floor_dbm = -108;
  RadioLinkStats b;
  b.valid = RadioLinkStats::kRssi | RadioLinkStats::kSnr;
  b.rssi_dbm = -91;
  b.snr_db = 7;
  modem_->PushStats(a);
  ASSERT_TRUE(WaitFor([&] { return l.link_stats() >= 1; }, 3000));
  EXPECT_EQ(l.last_link_stats().valid, a.valid);
  EXPECT_EQ(l.last_link_stats().rssi_dbm, -73);
  EXPECT_EQ(l.last_link_stats().noise_floor_dbm, -108);
  modem_->PushStats(b);
  ASSERT_TRUE(WaitFor([&] { return l.link_stats() >= 2; }, 3000));
  EXPECT_EQ(l.last_link_stats().valid, b.valid);
  EXPECT_EQ(l.last_link_stats().rssi_dbm, -91);
  EXPECT_EQ(l.last_link_stats().snr_db, 7);
  EXPECT_EQ(l.link_stats(), 2);  // each value exactly once
}

TEST_P(RadioDriverTest, MaxFrameSizeIsTheModemMtuAndLargerFramesAreRejected) {
  EXPECT_EQ(driver_->MaxFrameSize(), modem_->Caps().mtu);
  EXPECT_EQ(driver_->MaxFrameSize(), 255U);  // callable before Start()
  Make(64);
  EXPECT_EQ(driver_->MaxFrameSize(), 64U);
  RecordingListener l;
  ASSERT_TRUE(driver_->Start(l, {}).HasValue());
  const auto big = driver_->Send(Pattern(65));
  ASSERT_FALSE(big.HasValue());
  EXPECT_TRUE(IsTransportError(big.Error(), TransportErrc::kInvalidArgument));
  const auto edge = Pattern(64);
  ASSERT_TRUE(driver_->Send(edge).HasValue());
  EXPECT_EQ(modem_->PeerRead(64, 3000), edge);
  Make(0);  // no limit
  EXPECT_EQ(driver_->MaxFrameSize(), 0U);
}

TEST_P(RadioDriverTest, TxQueueIsOneFrameAndTxReadyIsCalledOnce) {
  RecordingListener l;
  ASSERT_TRUE(driver_->Start(l, {}).HasValue());
  modem_->HoldTransmit(true);
  ASSERT_TRUE(driver_->Send(Pattern(10, 1)).HasValue());
  const auto busy = driver_->Send(Pattern(10, 2));
  ASSERT_FALSE(busy.HasValue());
  EXPECT_TRUE(IsTransportError(busy.Error(), TransportErrc::kBusy));
  EXPECT_FALSE(driver_->Send(Pattern(10, 3)).HasValue());  // still one frame, still busy
  EXPECT_EQ(l.tx_ready(), 0);
  modem_->HoldTransmit(false);
  ASSERT_TRUE(WaitFor([&] { return l.tx_ready() >= 1; }, 3000));
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(l.tx_ready(), 1);
  ASSERT_TRUE(driver_->Send(Pattern(10, 4)).HasValue());
  const auto got = modem_->PeerRead(20, 3000);
  std::vector<std::uint8_t> want = Pattern(10, 1);
  const auto second = Pattern(10, 4);
  want.insert(want.end(), second.begin(), second.end());
  std::vector<std::uint8_t> all = got;
  while (all.size() < want.size()) {
    const auto more = modem_->PeerRead(want.size() - all.size(), 1000);
    if (more.empty()) {
      break;
    }
    all.insert(all.end(), more.begin(), more.end());
  }
  EXPECT_EQ(all, want);  // the rejected frames were never sent
}

TEST_P(RadioDriverTest, ConfigureErrorFailsStartAndRetryWorks) {
  modem_->FailConfigure(MakeTransportError(TransportErrc::kRetryable, "no module"));
  RecordingListener l;
  const auto r = driver_->Start(l, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
  EXPECT_GE(modem_->close_calls(), 1);
  EXPECT_FALSE(driver_->Send(Pattern(10)).HasValue());  // not started
  modem_->FailConfigure(std::nullopt);
  ASSERT_TRUE(driver_->Start(l, {}).HasValue());
  EXPECT_EQ(modem_->configure_calls(), 2);
  EXPECT_TRUE(driver_->Send(Pattern(10)).HasValue());
}

TEST_P(RadioDriverTest, ModemThreadingRulesAreKept) {
  RecordingListener l;
  ASSERT_TRUE(driver_->Start(l, {}).HasValue());
  std::size_t received_want = 0;
  for (int i = 0; i < 40; ++i) {
    const auto frame = Pattern(100, static_cast<std::uint8_t>(i));
    modem_->PeerWrite(frame);
    received_want += frame.size();
    while (!driver_->Send(frame).HasValue()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::ignore = modem_->PeerRead(1000, 100);
  }
  ASSERT_TRUE(WaitFor([&] { return l.received_size() >= received_want; }, 5000));
  EXPECT_EQ(modem_->overlaps(), 0) << "Transmit()/Receive() overlapped although the modem forbids it";
}

// RadioDriverContractTest comes with the header only for RecordingListener; the suite itself is instantiated in
// test_radio_driver_contract.
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(RadioDriverContractTest);

INSTANTIATE_TEST_SUITE_P(Duplex, RadioDriverTest, ::testing::Values(false, true),
                         [](const ::testing::TestParamInfo<bool>& i) {
                           return std::string{i.param ? "full_duplex" : "half_duplex"};
                         });

}  // namespace
