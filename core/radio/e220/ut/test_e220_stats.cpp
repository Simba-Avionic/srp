/**
 * Copyright (c) 2024
 *
 * @file test_e220_stats.cpp
 * @brief The optional RSSI/noise statistics of the E220 (driver file field `stats_period_ms`, default 0 = off):
 * the driver file, REG1 bit 5, the read command C0 C1 C2 C3 00 02 in the transparent mode and the reply cut
 * out of the received stream; OnLinkStats from RadioDriver. UNVERIFIED on a real module (see README.md).
 */
#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>  // NOLINT
#include <functional>
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/com/mavlink/testing/radio_driver_contract_test.h"
#include "core/radio/common/radio_driver.hpp"
#include "core/radio/e220/e220_modem.hpp"
#include "core/radio/e220/e220_registers.hpp"
#include "core/radio/e220/testing/fake_e220_module.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
using ara::com::mavlink::RadioLinkStats;
using ara::com::mavlink::testing::RadioDriverContractTest;
using ara::com::mavlink::testing::RecordingListener;
using testing::FakeE220Module;
using std::chrono::milliseconds;
using Clock = std::chrono::steady_clock;

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(RadioDriverContractTest);

const std::vector<std::uint8_t> kReadCommand{0xC0, 0xC1, 0xC2, 0xC3, 0x00, 0x02};

E220Timing FastTiming() {
  E220Timing t;
  t.aux_timeout = milliseconds(100);
  t.aux_poll = milliseconds(1);
  t.mode_switch_wait = milliseconds(2);
  t.mode_settle = milliseconds(0);
  t.command_timeout = milliseconds(100);
  t.retry_delay = milliseconds(5);
  t.read_slice = milliseconds(20);
  return t;
}

// ---------------------------------------------------------------- the driver file and the registers

TEST(E220StatsConfigTest, StatsPeriodDefaultsToOffAndIsParsed) {
  const std::string base = R"("uart": "/dev/ttyS1", "pins": {"m0": 1, "m1": 2, "aux": 3})";
  const auto off = ParseDriverConfigJson("{" + base + "}");
  ASSERT_TRUE(off.ok) << off.error;
  EXPECT_EQ(off.config.stats_period_ms, 0U);
  const auto on = ParseDriverConfigJson("{" + base + R"(, "stats_period_ms": 1000})");
  ASSERT_TRUE(on.ok) << on.error;
  EXPECT_EQ(on.config.stats_period_ms, 1000U);
}

TEST(E220StatsConfigTest, BadStatsPeriodNamesTheField) {
  const std::string base = R"("uart": "/dev/ttyS1", "pins": {"m0": 1, "m1": 2, "aux": 3})";
  for (const char* v : {"-1", "\"1000\"", "1.5", "3600001", "null"}) {
    const auto r = ParseDriverConfigJson("{" + base + ", \"stats_period_ms\": " + v + "}");
    ASSERT_FALSE(r.ok) << v;
    EXPECT_NE(r.error.find("stats_period_ms"), std::string::npos) << r.error;
  }
}

TEST(E220StatsConfigTest, AmbientRssiBitIsSetOnlyWhenTheStatisticsAreOn) {
  E220Config cfg;
  const Registers factory{0x00, 0x00, 0x62, 0x00, 0x17, 0x03};
  EXPECT_EQ(DesiredRegisters(cfg, factory)[kReg1] & 0x20, 0);
  cfg.ambient_rssi = true;
  const auto d = DesiredRegisters(cfg, factory);
  EXPECT_EQ(d[kReg1] & 0x20, 0x20);
  EXPECT_EQ(DifferingRegisters(factory, d, cfg), std::vector<std::size_t>{kReg1});
  EXPECT_TRUE(DifferingRegisters(d, d, cfg).empty());
}

// ---------------------------------------------------------------- the modem

class E220StatsTest : public ::testing::Test {
 protected:
  void SetUp() override { module_ = std::make_shared<FakeE220Module>(); }

  std::shared_ptr<E220Modem> Make(int stats_ms, E220Timing t = FastTiming()) {
    return std::make_shared<E220Modem>(
        "/dev/ttyTEST", E220Config{}, E220Pins{FakeE220Module::kPinM0, FakeE220Module::kPinM1, FakeE220Module::kPinAux},
        module_, module_, t, milliseconds(stats_ms));
  }
  // Receive() until `until` returns true or the timeout; returns everything received.
  std::vector<std::uint8_t> Pump(E220Modem* m, const std::function<bool()>& until, milliseconds timeout) {
    std::vector<std::uint8_t> all;
    const auto end = Clock::now() + timeout;
    while (Clock::now() < end && !until()) {
      auto r = m->Receive(milliseconds(5), {});
      EXPECT_TRUE(r.HasValue());
      if (r.HasValue()) {
        all.insert(all.end(), r.Value().begin(), r.Value().end());
      }
    }
    return all;
  }

  std::shared_ptr<FakeE220Module> module_;
};

TEST_F(E220StatsTest, OffByDefaultNoReadCommandsNoStatsAndTheRegisterBitIsClear) {
  auto m = Make(0);
  ASSERT_TRUE(m->Configure({}).HasValue());
  EXPECT_EQ(module_->RegistersNow()[kReg1] & 0x20, 0);
  Pump(m.get(), [] { return false; }, milliseconds(300));
  EXPECT_EQ(module_->rssi_commands.load(), 0);
  EXPECT_TRUE(module_->data_writes.empty()) << "nothing may be written to the UART in the stream";
  EXPECT_FALSE(m->TakeStats().has_value());
}

TEST_F(E220StatsTest, OnWritesReg1Bit5ThenReportsNoiseAndRssiPeriodically) {
  module_->noise_rssi = 0xB0;  // -80 dBm
  module_->last_rssi = 0xC4;   // -60 dBm
  auto m = Make(40);
  ASSERT_TRUE(m->Configure({}).HasValue());
  EXPECT_EQ(module_->written_registers, std::vector<int>{kReg1});
  EXPECT_EQ(module_->RegistersNow()[kReg1] & 0x20, 0x20);
  std::vector<RadioLinkStats> got;
  const auto t0 = Clock::now();
  const auto data = Pump(m.get(), [&] {
    if (const auto s = m->TakeStats()) {
      got.push_back(*s);
    }
    return got.size() >= 3;
  }, milliseconds(2000));
  const auto ms = std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count();
  ASSERT_EQ(got.size(), 3U);
  EXPECT_GE(ms, 2 * 40 - 10);  // three reports are at least two periods apart
  EXPECT_TRUE(data.empty()) << "the reply must not reach the data stream";
  for (const auto& s : got) {
    EXPECT_EQ(s.valid, RadioLinkStats::kRssi | RadioLinkStats::kNoiseFloor);
    EXPECT_EQ(s.noise_floor_dbm, -80);
    EXPECT_EQ(s.rssi_dbm, -60);
  }
  EXPECT_GE(module_->rssi_commands.load(), 3);
}

TEST_F(E220StatsTest, TheNewestValuesAreReported) {
  auto m = Make(20);
  ASSERT_TRUE(m->Configure({}).HasValue());
  module_->noise_rssi = 0xA0;  // -96
  module_->last_rssi = 0xD0;   // -48
  std::optional<RadioLinkStats> s;
  Pump(m.get(), [&] {
    s = m->TakeStats();
    return s.has_value();
  }, milliseconds(1000));
  ASSERT_TRUE(s.has_value());
  EXPECT_EQ(s->noise_floor_dbm, -96);
  EXPECT_EQ(s->rssi_dbm, -48);
}

TEST_F(E220StatsTest, TheReplyIsCutOutOfTheStreamWithTheDataAroundItIntact) {
  auto m = Make(30);
  ASSERT_TRUE(m->Configure({}).HasValue());
  // data from the air arrives together with the reply
  std::vector<std::uint8_t> payload;
  for (int i = 0; i < 40; ++i) {
    payload.push_back(static_cast<std::uint8_t>(i));
  }
  bool injected = false;
  int stats = 0;
  std::vector<std::uint8_t> all;
  const auto end = Clock::now() + milliseconds(1500);
  while (Clock::now() < end && (stats < 2 || all.size() < 2 * payload.size())) {
    if (module_->rssi_commands.load() > stats && !injected) {
      module_->InjectRx(payload);  // lands behind/around the reply in the UART stream
      injected = true;
    }
    auto r = m->Receive(milliseconds(5), {});
    ASSERT_TRUE(r.HasValue());
    all.insert(all.end(), r.Value().begin(), r.Value().end());
    if (m->TakeStats()) {
      ++stats;
      injected = false;
    }
  }
  ASSERT_GE(stats, 2);
  // only whole copies of the payload, nothing of the reply
  ASSERT_EQ(all.size() % payload.size(), 0U);
  for (std::size_t i = 0; i < all.size(); i += payload.size()) {
    EXPECT_TRUE(std::equal(payload.begin(), payload.end(), all.begin() + static_cast<std::ptrdiff_t>(i)));
  }
}

TEST_F(E220StatsTest, AReplySplitAcrossTwoReadsIsStillRecognised) {
  module_->split_rssi_reply = true;  // C1 00 | 02 noise rssi
  auto m = Make(30);
  ASSERT_TRUE(m->Configure({}).HasValue());
  std::optional<RadioLinkStats> s;
  const auto data = Pump(m.get(), [&] {
    s = m->TakeStats();
    return s.has_value();
  }, milliseconds(1000));
  ASSERT_TRUE(s.has_value());
  EXPECT_EQ(s->rssi_dbm, -60);
  EXPECT_TRUE(data.empty()) << "the first half of the reply leaked into the stream";
}

TEST_F(E220StatsTest, NoReplyGivesNoStatsAndNoDataLoss) {
  module_->answer_rssi_read = false;  // the module ignores the command (unverified behaviour)
  auto m = Make(30);
  ASSERT_TRUE(m->Configure({}).HasValue());
  const std::vector<std::uint8_t> airdata{0x10, 0x20, 0x30};
  module_->InjectRx(airdata);
  const auto data = Pump(m.get(), [] { return false; }, milliseconds(400));
  EXPECT_EQ(data, airdata);
  EXPECT_FALSE(m->TakeStats().has_value());
  EXPECT_GE(module_->rssi_commands.load(), 2);  // it keeps asking every period
}

TEST_F(E220StatsTest, AHeldPartialHeaderIsReleasedAsDataAfterTheCommandTimeout) {
  module_->answer_rssi_read = false;
  auto m = Make(30);
  ASSERT_TRUE(m->Configure({}).HasValue());
  // wait for a command to be pending, then data that looks like the start of a reply arrives
  Pump(m.get(), [&] { return module_->rssi_commands.load() > 0; }, milliseconds(500));
  module_->InjectRx({0x55, 0xC1, 0x00});
  const auto data = Pump(m.get(), [] { return false; }, milliseconds(500));
  EXPECT_EQ(data, (std::vector<std::uint8_t>{0x55, 0xC1, 0x00}));
}

TEST_F(E220StatsTest, TheCommandIsNeverSentWhileAuxIsBusy) {
  auto m = Make(10);
  ASSERT_TRUE(m->Configure({}).HasValue());
  module_->aux_stuck_busy = true;
  Pump(m.get(), [] { return false; }, milliseconds(150));
  EXPECT_EQ(module_->rssi_commands.load(), 0);
  module_->aux_stuck_busy = false;
  Pump(m.get(), [&] { return module_->rssi_commands.load() > 0; }, milliseconds(500));
  EXPECT_GT(module_->rssi_commands.load(), 0);
}

// ---------------------------------------------------------------- the driver

TEST_F(E220StatsTest, RadioDriverReportsOnLinkStatsAndKeepsTheData) {
  auto modem = Make(30);
  RadioDriver driver{"e220:test", modem};
  RecordingListener lis;
  ASSERT_TRUE(driver.Start(lis, {}).HasValue());
  const auto end = Clock::now() + milliseconds(2000);
  while (lis.link_stats() < 3 && Clock::now() < end) {
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_GE(lis.link_stats(), 3);
  const auto s = lis.last_link_stats();
  EXPECT_EQ(s.valid, RadioLinkStats::kRssi | RadioLinkStats::kNoiseFloor);
  EXPECT_EQ(s.noise_floor_dbm, -80);
  EXPECT_EQ(s.rssi_dbm, -60);
  module_->InjectRx({1, 2, 3, 4, 5});
  const auto end2 = Clock::now() + milliseconds(1000);
  while (lis.received_size() < 5 && Clock::now() < end2) {
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_EQ(lis.received(), (std::vector<std::uint8_t>{1, 2, 3, 4, 5}));
  driver.Stop();
}

TEST_F(E220StatsTest, RadioDriverWithoutStatsNeverCallsOnLinkStats) {
  auto modem = Make(0);
  RadioDriver driver{"e220:test", modem};
  RecordingListener lis;
  ASSERT_TRUE(driver.Start(lis, {}).HasValue());
  std::this_thread::sleep_for(milliseconds(300));
  driver.Stop();
  EXPECT_EQ(lis.link_stats(), 0);
  EXPECT_EQ(module_->rssi_commands.load(), 0);
}

}  // namespace
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
