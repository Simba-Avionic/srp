/**
 * Copyright (c) 2024
 *
 * @file test_e220_driver.cpp
 * @brief E220RadioDriver on the fake module: the Start() sequence, idempotency, AUX gating of the
 * TX path (both AUX polarities), Send/kBusy/OnTxReady, fatal errors, Stop, the factory.
 */
#include <gtest/gtest.h>

#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <chrono>  // NOLINT
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <tuple>
#include <vector>

#include "ara/com/mavlink/testing/radio_driver_contract_test.h"
#include "ara/com/mavlink/transport_error.h"
#include "core/radio/e220/e220_driver_factory.hpp"
#include "core/radio/e220/e220_radio_driver.hpp"
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

class E220Test : public ::testing::Test {
 protected:
  void SetUp() override { module_ = std::make_shared<FakeE220Module>(); }

  std::shared_ptr<E220RadioDriver> Make(const E220Config& cfg = {}, E220Timing t = FastTiming()) {
    return std::make_shared<E220RadioDriver>("lora", "/dev/ttyTEST", cfg, module_, module_, t);
  }
  std::size_t CountCommands(std::uint8_t op) {
    return static_cast<std::size_t>(std::count_if(
        module_->commands.begin(), module_->commands.end(),
        [op](const std::vector<std::uint8_t>& c) { return c[0] == op; }));
  }

  std::shared_ptr<FakeE220Module> module_;
  RecordingListener lis_;
};

// ---------------------------------------------------------------- Start: configuration

TEST_F(E220Test, FactoryModuleGetsOnlyDifferingRegisters) {
  E220Config cfg;
  cfg.channel = 18;
  cfg.uart_baudrate = 57600;
  cfg.power_dbm = 27;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->written_registers, (std::vector<int>{kReg0, kReg1, kReg2}));
  EXPECT_EQ(CountCommands(0xC0), 3U);
  EXPECT_EQ(module_->RegistersNow(), (Registers{0x00, 0x00, 0xC2, 0x01, 0x12, 0x03}));
  // sequence: read, 3 writes, verifying read
  ASSERT_EQ(module_->commands.size(), 5U);
  EXPECT_EQ(module_->commands[0], (std::vector<std::uint8_t>{0xC1, 0x00, 0x06}));
  EXPECT_EQ(module_->commands[1], (std::vector<std::uint8_t>{0xC0, 0x02, 0x01, 0xC2}));
  EXPECT_EQ(module_->commands[2], (std::vector<std::uint8_t>{0xC0, 0x03, 0x01, 0x01}));
  EXPECT_EQ(module_->commands[3], (std::vector<std::uint8_t>{0xC0, 0x04, 0x01, 0x12}));
  EXPECT_EQ(module_->commands[4], (std::vector<std::uint8_t>{0xC1, 0x00, 0x06}));
}

TEST_F(E220Test, ReadWithoutDifferencesWritesZeroBytesC0) {
  auto t = Make();  // the default configuration = the factory settings of the module
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(CountCommands(0xC0), 0U);
  EXPECT_EQ(module_->eeprom_writes.load(), 0);
  EXPECT_EQ(CountCommands(0xC1), 1U);  // only the read, no verification (nothing written)
}

TEST_F(E220Test, SecondStartIsIdempotent) {
  E220Config cfg;
  cfg.channel = 30;
  cfg.air_rate_bps = 9600;
  {
    auto t = Make(cfg);
    ASSERT_TRUE(t->Start(lis_, {}).HasValue());
    t->Stop();
  }
  const int writes_after_first = module_->eeprom_writes.load();
  EXPECT_GT(writes_after_first, 0);
  module_->commands.clear();
  auto t2 = Make(cfg);
  ASSERT_TRUE(t2->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->eeprom_writes.load(), writes_after_first);  // no new writes
  EXPECT_EQ(CountCommands(0xC0), 0U);
}

TEST_F(E220Test, ChangingOneParameterWritesOnlyThatRegister) {
  E220Config cfg;
  cfg.channel = 30;
  {
    auto t = Make(cfg);
    ASSERT_TRUE(t->Start(lis_, {}).HasValue());
    t->Stop();
  }
  module_->commands.clear();
  module_->written_registers.clear();
  cfg.channel = 31;
  auto t2 = Make(cfg);
  ASSERT_TRUE(t2->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->written_registers, std::vector<int>{kReg2});
  EXPECT_EQ(CountCommands(0xC0), 1U);
}

TEST_F(E220Test, Reg3IsForcedTransparentWithoutRssiByte) {
  module_->eeprom[kReg3] = 0xC3;  // RSSI byte + fixed transmission (would break MAVLink)
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->written_registers, std::vector<int>{kReg3});
  EXPECT_EQ(module_->RegistersNow()[kReg3], 0x03);
}

TEST_F(E220Test, Reg1AmbientRssiIsClearedAndBit2IsLeftAsRead) {
  module_->eeprom[kReg1] = 0x24;
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->written_registers, std::vector<int>{kReg1});
  EXPECT_EQ(module_->RegistersNow()[kReg1], 0x04);
}

TEST_F(E220Test, CryptRegistersAreNeverTouched) {
  E220Config cfg;
  cfg.channel = 5;
  cfg.address = 0x0A0B;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  for (const int r : module_->written_registers) {
    EXPECT_LT(r, 6);
  }
  EXPECT_EQ(module_->eeprom[6], 0);
  EXPECT_EQ(module_->eeprom[7], 0);
  for (const auto& c : module_->commands) {
    EXPECT_LE(c[1] + (c[0] == 0xC1 ? c[2] : 1), 6);  // range 00H-05H
  }
}

TEST_F(E220Test, LastRegistersReflectTheModuleAfterStart) {
  E220Config cfg;
  cfg.channel = 7;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(t->last_registers()[kReg2], 7);
}

// ---------------------------------------------------------------- modes, UART, baud rate

TEST_F(E220Test, ModeSequenceIsConfigThenNormal) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  ASSERT_EQ(module_->mode_log.size(), 2U);
  EXPECT_EQ(module_->mode_log[0], (std::pair<bool, bool>{true, true}));    // M0=1 M1=1
  EXPECT_EQ(module_->mode_log[1], (std::pair<bool, bool>{false, false}));  // M0=0 M1=0
}

TEST_F(E220Test, ConfigurationUsesUart9600And8N1ThenReopensAtConfiguredBaud) {
  E220Config cfg;
  cfg.uart_baudrate = 57600;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  ASSERT_EQ(module_->uart_opens.size(), 2U);
  EXPECT_EQ(module_->uart_opens[0].port, "/dev/ttyTEST");
  EXPECT_EQ(module_->uart_opens[0].baud, B9600);
  EXPECT_EQ(module_->uart_opens[0].parity, uart::Parity::kNone);
  EXPECT_EQ(module_->uart_opens[1].baud, B57600);  // reopened after the REG0 change
}

TEST_F(E220Test, Baud9600DoesNotReopenTheUart) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->uart_opens.size(), 1U);
}

// Covers the driver API (E220Config.parity -> IUartDriver::Open), not the JSON path: the config parser
// accepts 8N1 only, so no model produces a non-default parity yet.
TEST_F(E220Test, ParityFromConfigIsAppliedWhenReopening) {
  E220Config cfg;
  cfg.parity = UartParity::k8E1;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  ASSERT_EQ(module_->uart_opens.size(), 2U);
  EXPECT_EQ(module_->uart_opens[1].parity, uart::Parity::kEven);
  EXPECT_EQ(module_->uart_opens[1].baud, B9600);
}

// ---------------------------------------------------------------- errors and retries

TEST_F(E220Test, VerificationMismatchRetriesThreeTimesThenRetryableError) {
  module_->writes_do_not_stick = true;
  E220Config cfg;
  cfg.channel = 44;
  auto t = Make(cfg);
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
  EXPECT_NE(r.Error().SupportData().find("verification"), std::string::npos);
  EXPECT_NE(r.Error().SupportData().find("3 attempts"), std::string::npos);
  EXPECT_EQ(module_->read_commands.load(), 6);  // read + verification x 3 attempts
  EXPECT_EQ(module_->eeprom_writes.load(), 3);
  EXPECT_EQ(module_->mode_log.back(), (std::pair<bool, bool>{false, false}));  // the module returns to normal mode
}

TEST_F(E220Test, AuxStuckLowWaitsWithTimeoutAndFails) {
  module_->aux_stuck_busy = true;
  auto t = Make();
  const auto t0 = Clock::now();
  const auto r = t->Start(lis_, {});
  const auto ms = std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count();
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
  EXPECT_NE(r.Error().SupportData().find("AUX"), std::string::npos);
  EXPECT_GE(ms, 3 * 100 - 20);  // 3 attempts of the AUX timeout = 100 ms each
  EXPECT_EQ(module_->commands.size(), 0U);  // nothing sent to a busy module
}

TEST_F(E220Test, AuxReleasedLateIsWaitedFor) {
  module_->aux_stuck_busy = true;
  std::thread release{[this] {
    std::this_thread::sleep_for(milliseconds(40));
    module_->aux_stuck_busy = false;
  }};
  auto t = Make();
  const auto r = t->Start(lis_, {});
  release.join();
  EXPECT_TRUE(r.HasValue());
}

TEST_F(E220Test, FormatErrorResponseIsReported) {
  module_->format_error = true;
  auto t = Make();
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("FF FF FF"), std::string::npos);
}

TEST_F(E220Test, NoResponseTimesOut) {
  module_->ignore_commands = true;
  auto t = Make();
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("no response"), std::string::npos);
}

TEST_F(E220Test, UartOpenFailureIsRetried) {
  module_->fail_uart_open_count = 1;
  auto t = Make();
  EXPECT_TRUE(t->Start(lis_, {}).HasValue());
}

TEST_F(E220Test, UartOpenAlwaysFailingGivesRetryableError) {
  module_->fail_uart_open_count = 100;
  auto t = Make();
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
}

TEST_F(E220Test, PinFailureIsReported) {
  module_->pin_set_ok = false;
  auto t = Make();
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("M0/M1"), std::string::npos);
}

TEST_F(E220Test, InvalidConfigIsPersistentAndTouchesNoHardware) {
  E220Config cfg;
  cfg.channel = 99;
  auto t = Make(cfg);
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  EXPECT_TRUE(module_->uart_opens.empty());
  EXPECT_TRUE(module_->mode_log.empty());
}

TEST_F(E220Test, StopTokenAbortsStart) {
  std::stop_source src;
  src.request_stop();
  auto t = Make();
  const auto r = t->Start(lis_, src.get_token());
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kClosed));
}

TEST_F(E220Test, StopTokenDuringAuxWaitAbortsPromptly) {
  module_->aux_stuck_busy = true;
  E220Timing slow = FastTiming();
  slow.aux_timeout = milliseconds(5000);
  auto t = Make({}, slow);
  std::stop_source src;
  std::thread stopper{[&] {
    std::this_thread::sleep_for(milliseconds(60));
    src.request_stop();
  }};
  const auto t0 = Clock::now();
  const auto r = t->Start(lis_, src.get_token());
  stopper.join();
  EXPECT_FALSE(r.HasValue());
  EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 1000);
}

TEST_F(E220Test, StopDuringStartAbortsIt) {
  module_->aux_stuck_busy = true;
  E220Timing slow = FastTiming();
  slow.aux_timeout = milliseconds(5000);
  auto t = Make({}, slow);
  std::thread closer{[&] {
    std::this_thread::sleep_for(milliseconds(60));
    t->Stop();
  }};
  const auto t0 = Clock::now();
  const auto r = t->Start(lis_, {});
  closer.join();
  EXPECT_FALSE(r.HasValue());
  EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 1500);
}

// ---------------------------------------------------------------- Send / OnTxReady / RX

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

TEST_F(E220Test, SendDeliversAWholeShortFrameInOneUartWrite) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const std::vector<std::uint8_t> frame(100, 0x5A);
  ASSERT_TRUE(t->Send(frame).HasValue());
  EXPECT_EQ(DrainAir(module_.get(), frame.size(), milliseconds(2000)), frame);
  ASSERT_EQ(module_->data_writes.size(), 1U);
  t->Stop();
}

TEST_F(E220Test, FrameLongerThanPacketLengthIsWrittenInPortionsNotLargerThanIt) {
  E220Config cfg;
  cfg.packet_length = 32;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  std::vector<std::uint8_t> frame(100);
  for (std::size_t i = 0; i < frame.size(); ++i) {
    frame[i] = static_cast<std::uint8_t>(i);
  }
  ASSERT_TRUE(t->Send(frame).HasValue());
  EXPECT_EQ(DrainAir(module_.get(), frame.size(), milliseconds(2000)), frame);
  t->Stop();
  ASSERT_EQ(module_->data_writes.size(), 4U);  // 32 + 32 + 32 + 4
  for (const auto& w : module_->data_writes) {
    EXPECT_LE(w.size(), 32U);
  }
  EXPECT_EQ(module_->writes_while_busy.load(), 0);
}

TEST_F(E220Test, SendNeverBlocksEvenWhenAuxIsBusy) {
  module_->aux_busy_ms = 30;
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  for (int i = 0; i < 6; ++i) {
    const auto t0 = Clock::now();
    const auto r = t->Send(std::vector<std::uint8_t>(50, static_cast<std::uint8_t>(i)));
    EXPECT_TRUE(r.HasValue() || IsTransportError(r.Error(), TransportErrc::kBusy));
    EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 20);
  }
  t->Stop();
}

TEST_F(E220Test, AuxIsAwaitedBeforeEachUartWriteAndNothingIsWrittenWhileBusy) {
  module_->aux_busy_ms = 25;  // AUX busy for 25 ms after every write
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const auto t0 = Clock::now();
  for (int i = 0; i < 3; ++i) {
    ASSERT_TRUE(t->Send(std::vector<std::uint8_t>(40, static_cast<std::uint8_t>(i))).HasValue());
  }
  while (module_->data_writes.size() < 3 && Clock::now() - t0 < milliseconds(2000)) {
    std::this_thread::sleep_for(milliseconds(2));
  }
  ASSERT_EQ(module_->data_writes.size(), 3U);
  EXPECT_GE(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 2 * 25 - 10);  // 2 AUX pauses
  EXPECT_EQ(module_->writes_while_busy.load(), 0);
  t->Stop();
}

TEST_F(E220Test, BusyWhenTheQueueIsFullThenOneTxReadyWhenRoomAppears) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  module_->aux_stuck_busy = true;  // the module takes nothing: the queue only fills up
  const std::vector<std::uint8_t> frame(200, 1);
  ASSERT_TRUE(t->Send(frame).HasValue());
  ASSERT_TRUE(t->Send(frame).HasValue());  // 2 x 200 B = the whole 400 B queue
  const auto busy = t->Send(frame);
  ASSERT_FALSE(busy.HasValue());
  EXPECT_TRUE(IsTransportError(busy.Error(), TransportErrc::kBusy));
  EXPECT_TRUE(module_->data_writes.empty());  // zero writes while AUX is busy
  EXPECT_EQ(lis_.tx_ready(), 0);
  module_->aux_stuck_busy = false;  // the module is free: the first portion leaves the queue
  const auto end = Clock::now() + milliseconds(2000);
  while (lis_.tx_ready() == 0 && Clock::now() < end) {
    std::this_thread::sleep_for(milliseconds(2));
  }
  EXPECT_EQ(lis_.tx_ready(), 1);
  std::this_thread::sleep_for(milliseconds(100));
  EXPECT_EQ(lis_.tx_ready(), 1);  // one signal per kBusy
  EXPECT_TRUE(t->Send(frame).HasValue());
  t->Stop();
}

TEST_F(E220Test, FrameLargerThanTheQueueIsInvalidNotBusy) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const auto r = t->Send(std::vector<std::uint8_t>(401, 0));
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kInvalidArgument));
  t->Stop();
}

TEST_F(E220Test, AuxStuckBusyAfterStartIsFatalAndSendFailsAfterwards) {
  E220Timing t_aux = FastTiming();
  t_aux.aux_timeout = milliseconds(80);
  auto t = Make({}, t_aux);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  module_->aux_stuck_busy = true;
  ASSERT_TRUE(t->Send(std::vector<std::uint8_t>(20, 1)).HasValue());  // accepted into the queue
  const auto end = Clock::now() + milliseconds(2000);
  while (lis_.fatal_errors() == 0 && Clock::now() < end) {
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_EQ(lis_.fatal_errors(), 1);
  EXPECT_TRUE(module_->data_writes.empty());  // nothing reached the busy module
  const auto r = t->Send(std::vector<std::uint8_t>(20, 1));
  ASSERT_FALSE(r.HasValue());
  EXPECT_FALSE(IsTransportError(r.Error(), TransportErrc::kBusy));
  t->Stop();
}

TEST_F(E220Test, AuxThatNeverGoesBusyAfterAWriteIsFatal) {
  module_->aux_busy_ms = 0;  // the module never acknowledges the data
  E220Timing t_aux = FastTiming();
  t_aux.aux_timeout = milliseconds(60);
  auto t = Make({}, t_aux);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  ASSERT_TRUE(t->Send(std::vector<std::uint8_t>(20, 1)).HasValue());
  const auto end = Clock::now() + milliseconds(2000);
  while (lis_.fatal_errors() == 0 && Clock::now() < end) {
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_EQ(lis_.fatal_errors(), 1);
  t->Stop();
}

TEST_F(E220Test, UartWriteFailureIsFatal) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  module_->fail_uart_write = true;
  ASSERT_TRUE(t->Send(std::vector<std::uint8_t>(20, 1)).HasValue());
  const auto end = Clock::now() + milliseconds(2000);
  while (lis_.fatal_errors() == 0 && Clock::now() < end) {
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_EQ(lis_.fatal_errors(), 1);
  t->Stop();
}

// ---------------------------------------------------------------- AUX polarity

class E220PolarityTest : public E220Test, public ::testing::WithParamInterface<bool> {
 protected:
  void SetUp() override {
    E220Test::SetUp();
    module_->aux_idle_level = GetParam();
    cfg_.aux_idle_level = GetParam();
  }
  E220Config cfg_;
};

TEST_P(E220PolarityTest, StartAndGatedTransmissionWorkWithTheConfiguredIdleLevel) {
  module_->aux_busy_ms = 15;
  auto t = Make(cfg_);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const auto t0 = Clock::now();
  const std::vector<std::uint8_t> frame(150, 3);
  ASSERT_TRUE(t->Send(frame).HasValue());
  ASSERT_TRUE(t->Send(frame).HasValue());
  EXPECT_EQ(DrainAir(module_.get(), 2 * frame.size(), milliseconds(2000)).size(), 2 * frame.size());
  EXPECT_GE(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 15 - 5);
  EXPECT_EQ(module_->writes_while_busy.load(), 0);
  EXPECT_EQ(lis_.fatal_errors(), 0);
  t->Stop();
}

TEST_P(E220PolarityTest, BusyIsReportedWhenTheQueueIsFullAndNothingIsWrittenWhileBusy) {
  auto t = Make(cfg_);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  module_->aux_stuck_busy = true;
  const std::vector<std::uint8_t> frame(200, 1);
  ASSERT_TRUE(t->Send(frame).HasValue());
  ASSERT_TRUE(t->Send(frame).HasValue());
  EXPECT_TRUE(IsTransportError(t->Send(frame).Error(), TransportErrc::kBusy));
  EXPECT_TRUE(module_->data_writes.empty());
  t->Stop();
}

TEST_P(E220PolarityTest, TheOppositeLevelMeansBusyAndStartFailsWithAnAuxError) {
  cfg_.aux_idle_level = !GetParam();  // the driver expects the other level than the module idles at
  auto t = Make(cfg_);
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("AUX"), std::string::npos);
}

INSTANTIATE_TEST_SUITE_P(IdleLevel, E220PolarityTest, ::testing::Bool(),
                         [](const ::testing::TestParamInfo<bool>& i) { return i.param ? "High" : "Low"; });

TEST_F(E220Test, ReceivedBytesReachListenerAsChunksInOrder) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  std::vector<std::uint8_t> sent;
  for (int i = 0; i < 5; ++i) {
    const std::vector<std::uint8_t> part(30, static_cast<std::uint8_t>(i));
    module_->InjectRx(part);
    sent.insert(sent.end(), part.begin(), part.end());
    std::this_thread::sleep_for(milliseconds(10));
  }
  const auto end = Clock::now() + milliseconds(2000);
  while (lis_.received_size() < sent.size() && Clock::now() < end) {
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_EQ(lis_.received(), sent);
  t->Stop();
}

TEST_F(E220Test, UartReadFailureIsFatal) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  module_->uart_read_fails = true;
  module_->InjectRx({1, 2, 3});
  const auto end = Clock::now() + milliseconds(2000);
  while (lis_.fatal_errors() == 0 && Clock::now() < end) {
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_EQ(lis_.fatal_errors(), 1);
  t->Stop();
}

TEST_F(E220Test, SendBeforeStartAndAfterStopIsClosedError) {
  auto t = Make();
  EXPECT_TRUE(IsTransportError(t->Send(std::vector<std::uint8_t>(5, 0)).Error(), TransportErrc::kClosed));
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  t->Stop();
  EXPECT_TRUE(IsTransportError(t->Send(std::vector<std::uint8_t>(5, 0)).Error(), TransportErrc::kClosed));
  t->Stop();  // idempotentne
}

TEST_F(E220Test, StartAfterStopReconfiguresAndWorks) {
  E220Config cfg;
  cfg.channel = 30;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  t->Stop();
  const int writes = module_->eeprom_writes.load();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());  // the second start: no new register writes
  EXPECT_EQ(module_->eeprom_writes.load(), writes);
  ASSERT_TRUE(t->Send(std::vector<std::uint8_t>(10, 9)).HasValue());
  t->Stop();
}

TEST_F(E220Test, StartWithoutStopIsInvalid) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kInvalidArgument));
  t->Stop();
}

TEST_F(E220Test, StopWithQueuedFramesLeavesNoCallbacks) {
  module_->aux_busy_ms = 40;
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  for (int i = 0; i < 4; ++i) {
    std::ignore = t->Send(std::vector<std::uint8_t>(60, 1));
  }
  t->Stop();
  lis_.Seal();
  module_->InjectRx({1, 2, 3});
  std::this_thread::sleep_for(milliseconds(150));
  EXPECT_EQ(lis_.violations(), 0);
}

TEST_F(E220Test, StopWhileWaitingForTxReadyAfterBusyReturnsPromptlyAndLeavesNoCallbacks) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  module_->aux_stuck_busy = true;
  const std::vector<std::uint8_t> frame(200, 1);
  ASSERT_TRUE(t->Send(frame).HasValue());
  ASSERT_TRUE(t->Send(frame).HasValue());
  const auto busy = t->Send(frame);
  ASSERT_TRUE(IsTransportError(busy.Error(), TransportErrc::kBusy));
  std::this_thread::sleep_for(milliseconds(30));
  ASSERT_EQ(lis_.tx_ready(), 0);  // the TX thread waits for AUX idle: no OnTxReady yet
  const auto t0 = Clock::now();
  t->Stop();
  EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 1000)
      << "Stop() hung while waiting for AUX";
  lis_.Seal();
  module_->aux_stuck_busy = false;  // room would appear now, but the driver is stopped
  std::this_thread::sleep_for(milliseconds(150));
  EXPECT_EQ(lis_.tx_ready(), 0);
  EXPECT_EQ(lis_.violations(), 0) << "callback after Stop() returned";
  const auto after = t->Send(frame);
  ASSERT_FALSE(after.HasValue());
  EXPECT_TRUE(IsTransportError(after.Error(), TransportErrc::kClosed));
}

// ---------------------------------------------------------------- factory

std::string WriteTempConfig(const char* tag, const std::string& text) {
  const std::string path = std::string("/tmp/srp_e220_") + tag + "_" + std::to_string(::getpid()) + ".json";
  std::ofstream(path) << text;
  return path;
}

TEST_F(E220Test, FactoryWithABadConfigGivesADriverWhoseStartFailsPersistentlyWithTheReason) {
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

TEST_F(E220Test, FactoryWithoutAConfigPathOrWithAMissingFileFailsStart) {
  for (const std::string path : {std::string{}, std::string{"/tmp/srp_e220_missing_cfg.json"}}) {
    const auto d = CreateE220Driver(path);
    ASSERT_NE(d, nullptr);
    const auto r = d->Start(lis_, {});
    ASSERT_FALSE(r.HasValue());
    EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  }
}

TEST_F(E220Test, FactoryRejectsCtlDirPinsInTheProductionPlugin) {
  const auto path = WriteTempConfig("ctl", R"({"uart": "/dev/ttyS9", "pins": {"ctl_dir": "/tmp/sim"}})");
  const auto d = CreateE220Driver(path);  // allow_ctl_dir = false
  const auto r = d->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("test plugin"), std::string::npos) << r.Error().SupportData();
  std::remove(path.c_str());
}

TEST_F(E220Test, ConfigErrorDriverFailsStartPersistentlyWithTheMessage) {
  auto d = E220RadioDriver::WithConfigError("ebyte_e220", "file '/x.json': missing field 'uart'");
  const auto r = d->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  EXPECT_NE(r.Error().SupportData().find("missing field 'uart'"), std::string::npos);
  EXPECT_TRUE(IsTransportError(d->Send(std::vector<std::uint8_t>(1, 0)).Error(), TransportErrc::kClosed));
  d->Stop();
}

TEST_F(E220Test, NameContainsLinkName) {
  auto t = Make();
  EXPECT_NE(t->Name().find("lora"), std::string::npos);
}

}  // namespace
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
