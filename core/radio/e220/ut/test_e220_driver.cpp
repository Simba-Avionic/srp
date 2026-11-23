/**
 * Copyright (c) 2024
 *
 * @file test_e220_driver.cpp
 * @brief The `ebyte_e220` driver = RadioDriver + E220Modem on the fake module: Start() (config errors,
 * retries, a second start without C0 writes), AUX-gated Send/kBusy/OnTxReady, fatal errors, Stop and
 * the factory (driver file -> driver).
 */
#include <gtest/gtest.h>

#include <unistd.h>

#include <algorithm>
#include <chrono>  // NOLINT
#include <cstdio>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <thread>  // NOLINT
#include <tuple>
#include <vector>

#include "ara/com/mavlink/testing/radio_driver_contract_test.h"
#include "ara/com/mavlink/transport_error.h"
#include "core/radio/common/radio_driver.hpp"
#include "core/radio/e220/e220_driver_factory.hpp"
#include "core/radio/e220/e220_modem.hpp"
#include "core/radio/e220/testing/fake_e220_module.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
using ara::com::mavlink::IsTransportError;
using ara::com::mavlink::TransportErrc;
using ara::com::mavlink::testing::RadioDriverContractTest;
using ara::com::mavlink::testing::RecordingListener;
using testing::FakeE220Module;
using std::chrono::milliseconds;
using Clock = std::chrono::steady_clock;

// The contract suite header is included only for RecordingListener; the suite is instantiated in test_e220_contract.
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(RadioDriverContractTest);

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

class E220DriverTest : public ::testing::Test {
 protected:
  void SetUp() override { module_ = std::make_shared<FakeE220Module>(); }

  std::shared_ptr<RadioDriver> Make(const E220Config& cfg = {}, E220Timing t = FastTiming()) {
    auto modem = std::make_shared<E220Modem>(
        "/dev/ttyTEST", cfg, E220Pins{FakeE220Module::kPinM0, FakeE220Module::kPinM1, FakeE220Module::kPinAux},
        module_, module_, t);
    return std::make_shared<RadioDriver>("e220:test", modem);
  }
  void WaitFor(const std::function<bool()>& cond, milliseconds timeout = milliseconds(2000)) {
    const auto end = Clock::now() + timeout;
    while (!cond() && Clock::now() < end) {
      std::this_thread::sleep_for(milliseconds(2));
    }
  }

  std::shared_ptr<FakeE220Module> module_;
  RecordingListener lis_;
};

// Collects what the module received from the UART (as the "air") until `want` bytes arrived.
std::vector<std::uint8_t> DrainAir(FakeE220Module* m, std::size_t want, milliseconds timeout) {
  std::vector<std::uint8_t> got;
  const auto end = Clock::now() + timeout;
  while (got.size() < want && Clock::now() < end) {
    const auto part = m->DrainTx();
    got.insert(got.end(), part.begin(), part.end());
    std::this_thread::sleep_for(milliseconds(2));
  }
  return got;
}

// ---------------------------------------------------------------- Start

TEST_F(E220DriverTest, StartConfiguresTheModuleAndSecondStartWritesZeroBytesC0) {
  E220Config cfg;
  cfg.channel = 30;
  {
    auto d = Make(cfg);
    ASSERT_TRUE(d->Start(lis_, {}).HasValue());
    d->Stop();
  }
  const int writes = module_->eeprom_writes.load();
  EXPECT_GT(writes, 0);
  module_->commands.clear();
  auto d2 = Make(cfg);
  ASSERT_TRUE(d2->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->eeprom_writes.load(), writes);
  for (const auto& c : module_->commands) {
    EXPECT_NE(c[0], 0xC0) << "C0 sent on the second start";
  }
  d2->Stop();
}

TEST_F(E220DriverTest, VerificationMismatchRetriesThenRetryableErrorWithTheCause) {
  module_->writes_do_not_stick = true;
  E220Config cfg;
  cfg.channel = 44;
  auto d = Make(cfg);
  const auto r = d->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
  EXPECT_NE(r.Error().SupportData().find("verification"), std::string::npos);
  EXPECT_EQ(module_->mode_log.back(), (std::pair<bool, bool>{false, false}));
  EXPECT_FALSE(module_->uart_open());
}

TEST_F(E220DriverTest, InvalidConfigIsPersistentAndTouchesNoHardware) {
  E220Config cfg;
  cfg.channel = 99;
  auto d = Make(cfg);
  const auto r = d->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  EXPECT_TRUE(module_->uart_opens.empty());
}

TEST_F(E220DriverTest, StopTokenAbortsStart) {
  std::stop_source src;
  src.request_stop();
  auto d = Make();
  const auto r = d->Start(lis_, src.get_token());
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kClosed));
}

TEST_F(E220DriverTest, StopDuringStartAbortsIt) {
  module_->aux_stuck_busy = true;
  E220Timing slow = FastTiming();
  slow.aux_timeout = milliseconds(5000);
  auto d = Make({}, slow);
  std::thread closer{[&] {
    std::this_thread::sleep_for(milliseconds(60));
    d->Stop();
  }};
  const auto t0 = Clock::now();
  const auto r = d->Start(lis_, {});
  closer.join();
  EXPECT_FALSE(r.HasValue());
  EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 1500);
}

TEST_F(E220DriverTest, MaxFrameSizeIsThePacketLength) {
  E220Config cfg;
  cfg.packet_length = 128;
  EXPECT_EQ(Make(cfg)->MaxFrameSize(), 128U);
}

// ---------------------------------------------------------------- Send / OnTxReady / RX

TEST_F(E220DriverTest, SendDeliversAWholeShortFrameInOneUartWrite) {
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  const std::vector<std::uint8_t> frame(100, 0x5A);
  ASSERT_TRUE(d->Send(frame).HasValue());
  EXPECT_EQ(DrainAir(module_.get(), frame.size(), milliseconds(2000)), frame);
  ASSERT_EQ(module_->data_writes.size(), 1U);
  d->Stop();
}

TEST_F(E220DriverTest, FrameLargerThanThePacketLengthIsInvalid) {
  E220Config cfg;
  cfg.packet_length = 32;
  auto d = Make(cfg);
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  const auto r = d->Send(std::vector<std::uint8_t>(33, 0));
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kInvalidArgument));
  EXPECT_TRUE(d->Send(std::vector<std::uint8_t>(32, 0)).HasValue());
  d->Stop();
}

TEST_F(E220DriverTest, SendNeverBlocksEvenWhenAuxIsBusy) {
  module_->aux_busy_ms = 30;
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  for (int i = 0; i < 6; ++i) {
    const auto t0 = Clock::now();
    const auto r = d->Send(std::vector<std::uint8_t>(50, static_cast<std::uint8_t>(i)));
    EXPECT_TRUE(r.HasValue() || IsTransportError(r.Error(), TransportErrc::kBusy));
    EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 20);
  }
  d->Stop();
}

TEST_F(E220DriverTest, NothingIsWrittenWhileAuxIsBusyAndFramesGoOutOneAfterAnother) {
  module_->aux_busy_ms = 25;
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  const auto t0 = Clock::now();
  int sent = 0;
  while (sent < 3 && Clock::now() - t0 < milliseconds(3000)) {
    if (d->Send(std::vector<std::uint8_t>(40, static_cast<std::uint8_t>(sent))).HasValue()) {
      ++sent;
    } else {
      std::this_thread::sleep_for(milliseconds(2));
    }
  }
  WaitFor([&] { return module_->data_writes.size() >= 3; });
  ASSERT_EQ(module_->data_writes.size(), 3U);
  EXPECT_EQ(module_->writes_while_busy.load(), 0);
  d->Stop();
}

TEST_F(E220DriverTest, BusyWhenTheSlotIsTakenThenOneTxReadyWhenItFrees) {
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  module_->aux_stuck_busy = true;  // the module takes nothing: the frame stays in the slot
  const std::vector<std::uint8_t> frame(100, 1);
  ASSERT_TRUE(d->Send(frame).HasValue());
  const auto busy = d->Send(frame);
  ASSERT_FALSE(busy.HasValue());
  EXPECT_TRUE(IsTransportError(busy.Error(), TransportErrc::kBusy));
  EXPECT_TRUE(module_->data_writes.empty());  // zero writes while AUX is busy
  EXPECT_EQ(lis_.tx_ready(), 0);
  module_->aux_stuck_busy = false;  // the module is free: the frame leaves the slot
  WaitFor([&] { return lis_.tx_ready() > 0; });
  EXPECT_EQ(lis_.tx_ready(), 1);
  std::this_thread::sleep_for(milliseconds(100));
  EXPECT_EQ(lis_.tx_ready(), 1);  // one signal per kBusy
  EXPECT_TRUE(d->Send(frame).HasValue());
  d->Stop();
}

TEST_F(E220DriverTest, AuxStuckBusyAfterStartIsFatalAndSendFailsAfterwards) {
  E220Timing t_aux = FastTiming();
  t_aux.aux_timeout = milliseconds(80);
  auto d = Make({}, t_aux);
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  module_->aux_stuck_busy = true;
  ASSERT_TRUE(d->Send(std::vector<std::uint8_t>(20, 1)).HasValue());  // accepted into the slot
  WaitFor([&] { return lis_.fatal_errors() > 0; });
  EXPECT_EQ(lis_.fatal_errors(), 1);
  EXPECT_TRUE(module_->data_writes.empty());  // nothing reached the busy module
  const auto r = d->Send(std::vector<std::uint8_t>(20, 1));
  ASSERT_FALSE(r.HasValue());
  EXPECT_FALSE(IsTransportError(r.Error(), TransportErrc::kBusy));
  d->Stop();
}

TEST_F(E220DriverTest, AuxThatNeverGoesBusyAfterAWriteIsFatal) {
  module_->aux_busy_ms = 0;  // the module never acknowledges the data
  E220Timing t_aux = FastTiming();
  t_aux.aux_timeout = milliseconds(60);
  auto d = Make({}, t_aux);
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  ASSERT_TRUE(d->Send(std::vector<std::uint8_t>(20, 1)).HasValue());
  WaitFor([&] { return lis_.fatal_errors() > 0; });
  EXPECT_EQ(lis_.fatal_errors(), 1);
  d->Stop();
}

TEST_F(E220DriverTest, UartWriteFailureIsFatal) {
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  module_->fail_uart_write = true;
  ASSERT_TRUE(d->Send(std::vector<std::uint8_t>(20, 1)).HasValue());
  WaitFor([&] { return lis_.fatal_errors() > 0; });
  EXPECT_EQ(lis_.fatal_errors(), 1);
  d->Stop();
}

TEST_F(E220DriverTest, UartReadFailureIsFatal) {
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  module_->uart_read_fails = true;
  module_->InjectRx({1, 2, 3});
  WaitFor([&] { return lis_.fatal_errors() > 0; });
  EXPECT_EQ(lis_.fatal_errors(), 1);
  d->Stop();
}

TEST_F(E220DriverTest, FatalErrorThenStopStartWorksAgain) {
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  module_->fail_uart_write = true;
  ASSERT_TRUE(d->Send(std::vector<std::uint8_t>(20, 1)).HasValue());
  WaitFor([&] { return lis_.fatal_errors() > 0; });
  ASSERT_EQ(lis_.fatal_errors(), 1);
  d->Stop();
  module_->fail_uart_write = false;
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  EXPECT_TRUE(d->Send(std::vector<std::uint8_t>(20, 1)).HasValue());
  EXPECT_EQ(DrainAir(module_.get(), 20, milliseconds(2000)).size(), 20U);
  d->Stop();
}

// ---------------------------------------------------------------- AUX polarity

class E220DriverPolarityTest : public E220DriverTest, public ::testing::WithParamInterface<bool> {
 protected:
  void SetUp() override {
    E220DriverTest::SetUp();
    module_->aux_idle_level = GetParam();
    cfg_.aux_idle_level = GetParam();
  }
  E220Config cfg_;
};

TEST_P(E220DriverPolarityTest, StartAndGatedTransmissionWorkWithTheConfiguredIdleLevel) {
  module_->aux_busy_ms = 15;
  auto d = Make(cfg_);
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  const std::vector<std::uint8_t> frame(150, 3);
  ASSERT_TRUE(d->Send(frame).HasValue());
  WaitFor([&] { return module_->data_writes.size() >= 1; });
  WaitFor([&] { return d->Send(frame).HasValue(); });
  EXPECT_EQ(DrainAir(module_.get(), 2 * frame.size(), milliseconds(2000)).size(), 2 * frame.size());
  EXPECT_EQ(module_->writes_while_busy.load(), 0);
  EXPECT_EQ(lis_.fatal_errors(), 0);
  d->Stop();
}

TEST_P(E220DriverPolarityTest, BusyIsReportedWhenTheSlotIsTakenAndNothingIsWrittenWhileBusy) {
  auto d = Make(cfg_);
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  module_->aux_stuck_busy = true;
  const std::vector<std::uint8_t> frame(100, 1);
  ASSERT_TRUE(d->Send(frame).HasValue());
  EXPECT_TRUE(IsTransportError(d->Send(frame).Error(), TransportErrc::kBusy));
  EXPECT_TRUE(module_->data_writes.empty());
  d->Stop();
}

INSTANTIATE_TEST_SUITE_P(IdleLevel, E220DriverPolarityTest, ::testing::Bool(),
                         [](const ::testing::TestParamInfo<bool>& i) { return i.param ? "High" : "Low"; });

// ---------------------------------------------------------------- RX, lifecycle

TEST_F(E220DriverTest, ReceivedBytesReachListenerInOrder) {
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  std::vector<std::uint8_t> sent;
  for (int i = 0; i < 5; ++i) {
    const std::vector<std::uint8_t> part(30, static_cast<std::uint8_t>(i));
    module_->InjectRx(part);
    sent.insert(sent.end(), part.begin(), part.end());
    std::this_thread::sleep_for(milliseconds(10));
  }
  WaitFor([&] { return lis_.received_size() >= sent.size(); });
  EXPECT_EQ(lis_.received(), sent);
  d->Stop();
}

TEST_F(E220DriverTest, SendBeforeStartAndAfterStopIsClosedError) {
  auto d = Make();
  EXPECT_TRUE(IsTransportError(d->Send(std::vector<std::uint8_t>(5, 0)).Error(), TransportErrc::kClosed));
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  d->Stop();
  EXPECT_TRUE(IsTransportError(d->Send(std::vector<std::uint8_t>(5, 0)).Error(), TransportErrc::kClosed));
  d->Stop();  // idempotent
}

TEST_F(E220DriverTest, StartWithoutStopIsInvalid) {
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  const auto r = d->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kInvalidArgument));
  d->Stop();
}

TEST_F(E220DriverTest, StopWithAQueuedFrameLeavesNoCallbacks) {
  module_->aux_busy_ms = 40;
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  for (int i = 0; i < 4; ++i) {
    std::ignore = d->Send(std::vector<std::uint8_t>(60, 1));
  }
  d->Stop();
  lis_.Seal();
  module_->InjectRx({1, 2, 3});
  std::this_thread::sleep_for(milliseconds(150));
  EXPECT_EQ(lis_.violations(), 0);
}

TEST_F(E220DriverTest, StopWhileWaitingForTxReadyAfterBusyReturnsPromptlyAndLeavesNoCallbacks) {
  auto d = Make();
  ASSERT_TRUE(d->Start(lis_, {}).HasValue());
  module_->aux_stuck_busy = true;
  const std::vector<std::uint8_t> frame(100, 1);
  ASSERT_TRUE(d->Send(frame).HasValue());
  ASSERT_TRUE(IsTransportError(d->Send(frame).Error(), TransportErrc::kBusy));
  std::this_thread::sleep_for(milliseconds(30));
  ASSERT_EQ(lis_.tx_ready(), 0);  // the thread waits for AUX idle: no OnTxReady yet
  const auto t0 = Clock::now();
  d->Stop();
  EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 1000) << "Stop() hung on AUX";
  lis_.Seal();
  module_->aux_stuck_busy = false;
  std::this_thread::sleep_for(milliseconds(150));
  EXPECT_EQ(lis_.tx_ready(), 0);
  EXPECT_EQ(lis_.violations(), 0) << "callback after Stop() returned";
  EXPECT_TRUE(IsTransportError(d->Send(frame).Error(), TransportErrc::kClosed));
}

// ---------------------------------------------------------------- factory

std::string WriteTempConfig(const char* tag, const std::string& text) {
  const std::string path = std::string("/tmp/srp_e220_") + tag + "_" + std::to_string(::getpid()) + ".json";
  std::ofstream(path) << text;
  return path;
}

TEST_F(E220DriverTest, FactoryWithABadConfigGivesADriverWhoseStartFailsPersistentlyWithTheReason) {
  const auto path = WriteTempConfig(
      "bad", R"({"uart": "/dev/ttyS9", "pins": {"m0": 1, "m1": 2, "aux": 3}, "e220": {"channel": 200}})");
  const auto d = CreateE220Driver(path);
  ASSERT_NE(d, nullptr);
  const auto r = d->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  EXPECT_NE(r.Error().SupportData().find(path), std::string::npos) << r.Error().SupportData();
  EXPECT_NE(r.Error().SupportData().find("e220:"), std::string::npos) << r.Error().SupportData();
  d->Stop();
  std::remove(path.c_str());
}

TEST_F(E220DriverTest, FactoryWithoutAConfigPathOrWithAMissingFileFailsStart) {
  for (const std::string path : {std::string{}, std::string{"/tmp/srp_e220_missing_cfg.json"}}) {
    const auto d = CreateE220Driver(path);
    ASSERT_NE(d, nullptr);
    const auto r = d->Start(lis_, {});
    ASSERT_FALSE(r.HasValue());
    EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  }
}

TEST_F(E220DriverTest, FactoryRejectsCtlDirPinsOnTheRealHardware) {
  const auto path = WriteTempConfig("ctl", R"({"uart": "/dev/ttyS9", "pins": {"ctl_dir": "/tmp/sim"}})");
  const auto d = CreateE220Driver(path);
  const auto r = d->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  EXPECT_NE(r.Error().SupportData().find("test driver"), std::string::npos) << r.Error().SupportData();
  std::remove(path.c_str());
}

TEST_F(E220DriverTest, FactoryGivesTheNumbersOfTheDriverFileToTheGpioDriver) {
  const auto path = WriteTempConfig(
      "ok", R"({"uart": "/dev/ttyS9", "pins": {"m0": 1, "m1": 2, "aux": 3}, "e220": {"channel": 20}})");
  bool called = false;
  const auto hw = [&](const E220DriverConfig& cfg, std::string*) -> std::optional<E220Hardware> {
    called = true;
    EXPECT_EQ(cfg.uart, "/dev/ttyS9");
    return E220Hardware{module_, module_};
  };
  // The fake module has its pins at 17/27/22, the driver file says 1/2/3: the fake rejects them, so Start()
  // fails (AUX cannot be read) - which proves that the numbers of the file reach the GPIO driver.
  const auto d = CreateE220Driver(path, hw, FastTiming());
  ASSERT_NE(d, nullptr);
  const auto r = d->Start(lis_, {});
  EXPECT_TRUE(called);
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("AUX"), std::string::npos) << r.Error().SupportData();
  std::remove(path.c_str());
}

TEST_F(E220DriverTest, DriverNameContainsTheUart) {
  const auto path = WriteTempConfig("name", R"({"uart": "/dev/ttyS7", "pins": {"m0": 1, "m1": 2, "aux": 3}})");
  const auto d = CreateE220Driver(path);
  EXPECT_NE(d->Name().find("/dev/ttyS7"), std::string::npos);
  EXPECT_NE(d->Name().find("e220"), std::string::npos);
  std::remove(path.c_str());
}

}  // namespace
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
