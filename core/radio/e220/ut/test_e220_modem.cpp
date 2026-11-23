/**
 * Copyright (c) 2024
 *
 * @file test_e220_modem.cpp
 * @brief E220Modem on the fake module: the Configure() sequence, idempotency, the AUX gating of
 * Transmit() (both AUX polarities), Receive(), errors and the stop token.
 */
#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>  // NOLINT
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/com/mavlink/transport_error.h"
#include "core/radio/e220/e220_modem.hpp"
#include "core/radio/e220/testing/fake_e220_module.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
using ara::com::mavlink::IsTransportError;
using ara::com::mavlink::TransportErrc;
using testing::FakeE220Module;
using std::chrono::milliseconds;
using Clock = std::chrono::steady_clock;

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

class E220ModemTest : public ::testing::Test {
 protected:
  void SetUp() override { module_ = std::make_shared<FakeE220Module>(); }

  std::shared_ptr<E220Modem> Make(const E220Config& cfg = {}, E220Timing t = FastTiming()) {
    return std::make_shared<E220Modem>(
        "/dev/ttyTEST", cfg, E220Pins{FakeE220Module::kPinM0, FakeE220Module::kPinM1, FakeE220Module::kPinAux},
        module_, module_, t);
  }
  std::size_t CountCommands(std::uint8_t op) {
    return static_cast<std::size_t>(std::count_if(module_->commands.begin(), module_->commands.end(),
                                                  [op](const std::vector<std::uint8_t>& c) { return c[0] == op; }));
  }

  std::shared_ptr<FakeE220Module> module_;
};

// ---------------------------------------------------------------- Configure: registers

TEST_F(E220ModemTest, FactoryModuleGetsOnlyDifferingRegisters) {
  E220Config cfg;
  cfg.channel = 18;
  cfg.uart_baudrate = 57600;
  cfg.power_dbm = 27;
  auto m = Make(cfg);
  ASSERT_TRUE(m->Configure({}).HasValue());
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

TEST_F(E220ModemTest, ReadWithoutDifferencesWritesZeroBytesC0) {
  auto m = Make();  // the default configuration = the factory settings of the module
  ASSERT_TRUE(m->Configure({}).HasValue());
  EXPECT_EQ(CountCommands(0xC0), 0U);
  EXPECT_EQ(module_->eeprom_writes.load(), 0);
  EXPECT_EQ(CountCommands(0xC1), 1U);  // only the read, no verification (nothing written)
}

TEST_F(E220ModemTest, SecondConfigureIsIdempotent) {
  E220Config cfg;
  cfg.channel = 30;
  cfg.air_rate_bps = 9600;
  {
    auto m = Make(cfg);
    ASSERT_TRUE(m->Configure({}).HasValue());
    m->Close();
  }
  const int writes_after_first = module_->eeprom_writes.load();
  EXPECT_GT(writes_after_first, 0);
  module_->commands.clear();
  auto m2 = Make(cfg);
  ASSERT_TRUE(m2->Configure({}).HasValue());
  EXPECT_EQ(module_->eeprom_writes.load(), writes_after_first);  // no new writes
  EXPECT_EQ(CountCommands(0xC0), 0U);
}

TEST_F(E220ModemTest, ChangingOneParameterWritesOnlyThatRegister) {
  E220Config cfg;
  cfg.channel = 30;
  {
    auto m = Make(cfg);
    ASSERT_TRUE(m->Configure({}).HasValue());
    m->Close();
  }
  module_->commands.clear();
  module_->written_registers.clear();
  cfg.channel = 31;
  auto m2 = Make(cfg);
  ASSERT_TRUE(m2->Configure({}).HasValue());
  EXPECT_EQ(module_->written_registers, std::vector<int>{kReg2});
  EXPECT_EQ(CountCommands(0xC0), 1U);
}

TEST_F(E220ModemTest, Reg3IsForcedTransparentWithoutRssiByte) {
  module_->eeprom[kReg3] = 0xC3;  // RSSI byte + fixed transmission (would break MAVLink)
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  EXPECT_EQ(module_->written_registers, std::vector<int>{kReg3});
  EXPECT_EQ(module_->RegistersNow()[kReg3], 0x03);
}

TEST_F(E220ModemTest, Reg1AmbientRssiIsClearedAndBit2IsLeftAsRead) {
  module_->eeprom[kReg1] = 0x24;
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  EXPECT_EQ(module_->written_registers, std::vector<int>{kReg1});
  EXPECT_EQ(module_->RegistersNow()[kReg1], 0x04);
}

TEST_F(E220ModemTest, CryptRegistersAreNeverTouched) {
  E220Config cfg;
  cfg.channel = 5;
  cfg.address = 0x0A0B;
  auto m = Make(cfg);
  ASSERT_TRUE(m->Configure({}).HasValue());
  for (const int r : module_->written_registers) {
    EXPECT_LT(r, 6);
  }
  EXPECT_EQ(module_->eeprom[6], 0);
  EXPECT_EQ(module_->eeprom[7], 0);
  for (const auto& c : module_->commands) {
    EXPECT_LE(c[1] + (c[0] == 0xC1 ? c[2] : 1), 6);  // range 00H-05H
  }
}

TEST_F(E220ModemTest, LastRegistersReflectTheModuleAfterConfigure) {
  E220Config cfg;
  cfg.channel = 7;
  auto m = Make(cfg);
  ASSERT_TRUE(m->Configure({}).HasValue());
  EXPECT_EQ(m->last_registers()[kReg2], 7);
}

TEST_F(E220ModemTest, CapsAreTheHalfDuplexStreamWithTheMtuOfTheConfiguredPacketLength) {
  E220Config cfg;
  cfg.packet_length = 64;
  const auto caps = Make(cfg)->Caps();  // valid before Configure()
  EXPECT_EQ(caps.mtu, 64U);
  EXPECT_FALSE(caps.packet_based);
  EXPECT_FALSE(caps.full_duplex);
}

// ---------------------------------------------------------------- modes, UART, baud rate

TEST_F(E220ModemTest, ModeSequenceIsConfigThenNormal) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  ASSERT_EQ(module_->mode_log.size(), 2U);
  EXPECT_EQ(module_->mode_log[0], (std::pair<bool, bool>{true, true}));    // M0=1 M1=1
  EXPECT_EQ(module_->mode_log[1], (std::pair<bool, bool>{false, false}));  // M0=0 M1=0
}

TEST_F(E220ModemTest, ConfigurationUsesUart9600And8N1ThenReopensAtConfiguredBaud) {
  E220Config cfg;
  cfg.uart_baudrate = 57600;
  auto m = Make(cfg);
  ASSERT_TRUE(m->Configure({}).HasValue());
  ASSERT_EQ(module_->uart_opens.size(), 2U);
  EXPECT_EQ(module_->uart_opens[0].port, "/dev/ttyTEST");
  EXPECT_EQ(module_->uart_opens[0].baud, B9600);
  EXPECT_EQ(module_->uart_opens[0].parity, uart::Parity::kNone);
  EXPECT_EQ(module_->uart_opens[1].baud, B57600);  // reopened after the REG0 change
}

TEST_F(E220ModemTest, Baud9600DoesNotReopenTheUart) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  EXPECT_EQ(module_->uart_opens.size(), 1U);
}

// Covers E220Config.parity -> IUartDriver::Open (UartDriver supports parity), not the JSON path: the
// driver file parser still accepts 8N1 only.
TEST_F(E220ModemTest, ParityFromConfigIsAppliedWhenReopening) {
  E220Config cfg;
  cfg.parity = UartParity::k8E1;
  auto m = Make(cfg);
  ASSERT_TRUE(m->Configure({}).HasValue());
  ASSERT_EQ(module_->uart_opens.size(), 2U);
  EXPECT_EQ(module_->uart_opens[1].parity, uart::Parity::kEven);
  EXPECT_EQ(module_->uart_opens[1].baud, B9600);
}

TEST_F(E220ModemTest, ModeIsSwitchedThroughTheGpioDriverOnTheConfiguredPins) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  EXPECT_EQ(module_->getDirection(FakeE220Module::kPinAux), gpio::direction_t::IN);
  EXPECT_EQ(module_->getDirection(FakeE220Module::kPinM0), gpio::direction_t::OUT);
}

TEST_F(E220ModemTest, AnAlreadyExportedPinDoesNotFailConfigure) {
  // initializePin() fails (the pin is exported from the previous run), setDirection() works.
  class ExportedPins : public FakeE220Module {
   public:
    core::ErrorCode initializePin(const uint16_t&, const gpio::direction_t&, bool = true) override {
      return core::ErrorCode::kError;
    }
  };
  auto module = std::make_shared<ExportedPins>();
  E220Modem m{"/dev/ttyTEST", E220Config{},
              E220Pins{FakeE220Module::kPinM0, FakeE220Module::kPinM1, FakeE220Module::kPinAux}, module, module,
              FastTiming()};
  EXPECT_TRUE(m.Configure({}).HasValue());
}

// ---------------------------------------------------------------- errors and retries

TEST_F(E220ModemTest, VerificationMismatchRetriesThreeTimesThenRetryableError) {
  module_->writes_do_not_stick = true;
  E220Config cfg;
  cfg.channel = 44;
  auto m = Make(cfg);
  const auto r = m->Configure({});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
  EXPECT_NE(r.Error().SupportData().find("verification"), std::string::npos);
  EXPECT_NE(r.Error().SupportData().find("3 attempts"), std::string::npos);
  EXPECT_EQ(module_->read_commands.load(), 6);  // read + verification x 3 attempts
  EXPECT_EQ(module_->eeprom_writes.load(), 3);
  EXPECT_EQ(module_->mode_log.back(), (std::pair<bool, bool>{false, false}));  // the module returns to normal mode
}

TEST_F(E220ModemTest, AuxStuckLowWaitsWithTimeoutAndFails) {
  module_->aux_stuck_busy = true;
  auto m = Make();
  const auto t0 = Clock::now();
  const auto r = m->Configure({});
  const auto ms = std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count();
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
  EXPECT_NE(r.Error().SupportData().find("AUX"), std::string::npos);
  EXPECT_GE(ms, 3 * 100 - 20);  // 3 attempts of the AUX timeout = 100 ms each
  EXPECT_EQ(module_->commands.size(), 0U);  // nothing sent to a busy module
}

TEST_F(E220ModemTest, AuxReleasedLateIsWaitedFor) {
  module_->aux_stuck_busy = true;
  std::thread release{[this] {
    std::this_thread::sleep_for(milliseconds(40));
    module_->aux_stuck_busy = false;
  }};
  auto m = Make();
  const auto r = m->Configure({});
  release.join();
  EXPECT_TRUE(r.HasValue());
}

TEST_F(E220ModemTest, FormatErrorResponseIsReported) {
  module_->format_error = true;
  auto m = Make();
  const auto r = m->Configure({});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("FF FF FF"), std::string::npos);
}

TEST_F(E220ModemTest, NoResponseTimesOut) {
  module_->ignore_commands = true;
  auto m = Make();
  const auto r = m->Configure({});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("no response"), std::string::npos);
}

TEST_F(E220ModemTest, UartOpenFailureIsRetried) {
  module_->fail_uart_open_count = 1;
  auto m = Make();
  EXPECT_TRUE(m->Configure({}).HasValue());
}

TEST_F(E220ModemTest, UartOpenAlwaysFailingGivesRetryableError) {
  module_->fail_uart_open_count = 100;
  auto m = Make();
  const auto r = m->Configure({});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
}

TEST_F(E220ModemTest, PinFailureIsReported) {
  module_->pin_set_ok = false;
  auto m = Make();
  const auto r = m->Configure({});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("M0/M1"), std::string::npos);
}

TEST_F(E220ModemTest, PinSetupFailureIsReportedWithThePin) {
  module_->pin_init_ok = false;
  auto m = Make();
  const auto r = m->Configure({});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("GPIO"), std::string::npos) << r.Error().SupportData();
}

TEST_F(E220ModemTest, InvalidConfigIsPersistentAndTouchesNoHardware) {
  E220Config cfg;
  cfg.channel = 99;
  auto m = Make(cfg);
  const auto r = m->Configure({});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  EXPECT_TRUE(module_->uart_opens.empty());
  EXPECT_TRUE(module_->mode_log.empty());
}

TEST_F(E220ModemTest, ConfigErrorModemFailsPersistentlyWithTheMessage) {
  auto m = E220Modem::WithConfigError("file '/x.json': missing field 'uart'");
  const auto r = m->Configure({});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  EXPECT_NE(r.Error().SupportData().find("missing field 'uart'"), std::string::npos);
  m->Close();  // no hardware: harmless
}

TEST_F(E220ModemTest, StopTokenAbortsConfigure) {
  std::stop_source src;
  src.request_stop();
  auto m = Make();
  const auto r = m->Configure(src.get_token());
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kClosed));
}

TEST_F(E220ModemTest, StopTokenDuringAuxWaitAbortsPromptly) {
  module_->aux_stuck_busy = true;
  E220Timing slow = FastTiming();
  slow.aux_timeout = milliseconds(5000);
  auto m = Make({}, slow);
  std::stop_source src;
  std::thread stopper{[&] {
    std::this_thread::sleep_for(milliseconds(60));
    src.request_stop();
  }};
  const auto t0 = Clock::now();
  const auto r = m->Configure(src.get_token());
  stopper.join();
  EXPECT_FALSE(r.HasValue());
  EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 1000);
}

// ---------------------------------------------------------------- Transmit: the AUX gate

TEST_F(E220ModemTest, TransmitWritesAWholeShortFrameInOneUartWrite) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  const std::vector<std::uint8_t> frame(100, 0x5A);
  ASSERT_TRUE(m->Transmit(frame, {}).HasValue());
  EXPECT_EQ(module_->DrainTx(), frame);
  ASSERT_EQ(module_->data_writes.size(), 1U);
}

TEST_F(E220ModemTest, FrameLongerThanPacketLengthIsWrittenInPortionsNotLargerThanIt) {
  E220Config cfg;
  cfg.packet_length = 32;
  auto m = Make(cfg);
  ASSERT_TRUE(m->Configure({}).HasValue());
  std::vector<std::uint8_t> frame(100);
  for (std::size_t i = 0; i < frame.size(); ++i) {
    frame[i] = static_cast<std::uint8_t>(i);
  }
  ASSERT_TRUE(m->Transmit(frame, {}).HasValue());  // direct call: RadioDriver never passes more than the MTU
  EXPECT_EQ(module_->DrainTx(), frame);
  ASSERT_EQ(module_->data_writes.size(), 4U);  // 32 + 32 + 32 + 4
  for (const auto& w : module_->data_writes) {
    EXPECT_LE(w.size(), 32U);
  }
  EXPECT_EQ(module_->writes_while_busy.load(), 0);
}

TEST_F(E220ModemTest, AuxIsAwaitedBeforeEachUartWriteAndNothingIsWrittenWhileBusy) {
  module_->aux_busy_ms = 25;  // AUX busy for 25 ms after every write
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  const auto t0 = Clock::now();
  for (int i = 0; i < 3; ++i) {
    ASSERT_TRUE(m->Transmit(std::vector<std::uint8_t>(40, static_cast<std::uint8_t>(i)), {}).HasValue());
  }
  ASSERT_EQ(module_->data_writes.size(), 3U);
  // waits for each AUX cycle
  EXPECT_GE(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 3 * 25 - 10);
  EXPECT_EQ(module_->writes_while_busy.load(), 0);
}

TEST_F(E220ModemTest, TransmitWaitsWhileAuxIsBusyAndNothingIsWritten) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  module_->aux_stuck_busy = true;
  std::thread release{[this] {
    std::this_thread::sleep_for(milliseconds(40));
    module_->aux_stuck_busy = false;
  }};
  EXPECT_TRUE(m->Transmit(std::vector<std::uint8_t>(30, 1), {}).HasValue());
  release.join();
  EXPECT_EQ(module_->writes_while_busy.load(), 0);
}

TEST_F(E220ModemTest, AuxStuckBusyBeforeTheWriteIsPersistentAndNothingReachesTheModule) {
  E220Timing t_aux = FastTiming();
  t_aux.aux_timeout = milliseconds(80);
  auto m = Make({}, t_aux);
  ASSERT_TRUE(m->Configure({}).HasValue());
  module_->aux_stuck_busy = true;
  const auto r = m->Transmit(std::vector<std::uint8_t>(20, 1), {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  EXPECT_NE(r.Error().SupportData().find("AUX"), std::string::npos);
  EXPECT_TRUE(module_->data_writes.empty());
}

TEST_F(E220ModemTest, AuxThatNeverGoesBusyAfterAWriteIsPersistent) {
  module_->aux_busy_ms = 0;  // the module never acknowledges the data
  E220Timing t_aux = FastTiming();
  t_aux.aux_timeout = milliseconds(60);
  auto m = Make({}, t_aux);
  ASSERT_TRUE(m->Configure({}).HasValue());
  const auto r = m->Transmit(std::vector<std::uint8_t>(20, 1), {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
}

TEST_F(E220ModemTest, UartWriteFailureIsPersistent) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  module_->fail_uart_write = true;
  const auto r = m->Transmit(std::vector<std::uint8_t>(20, 1), {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
}

TEST_F(E220ModemTest, StopInterruptsATransmitWaitingForAux) {
  auto m = Make({}, [] {
    E220Timing t = FastTiming();
    t.aux_timeout = milliseconds(5000);
    return t;
  }());
  ASSERT_TRUE(m->Configure({}).HasValue());
  module_->aux_stuck_busy = true;
  std::stop_source src;
  std::thread stopper{[&] {
    std::this_thread::sleep_for(milliseconds(50));
    src.request_stop();
  }};
  const auto t0 = Clock::now();
  const auto r = m->Transmit(std::vector<std::uint8_t>(20, 1), src.get_token());
  stopper.join();
  EXPECT_FALSE(r.HasValue());
  EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 1000);
  EXPECT_TRUE(module_->data_writes.empty());
}

// ---------------------------------------------------------------- AUX polarity

class E220ModemPolarityTest : public E220ModemTest, public ::testing::WithParamInterface<bool> {
 protected:
  void SetUp() override {
    E220ModemTest::SetUp();
    module_->aux_idle_level = GetParam();
    cfg_.aux_idle_level = GetParam();
  }
  E220Config cfg_;
};

TEST_P(E220ModemPolarityTest, ConfigureAndGatedTransmissionWorkWithTheConfiguredIdleLevel) {
  module_->aux_busy_ms = 15;
  auto m = Make(cfg_);
  ASSERT_TRUE(m->Configure({}).HasValue());
  const auto t0 = Clock::now();
  const std::vector<std::uint8_t> frame(150, 3);
  ASSERT_TRUE(m->Transmit(frame, {}).HasValue());
  ASSERT_TRUE(m->Transmit(frame, {}).HasValue());
  EXPECT_EQ(module_->DrainTx().size(), 2 * frame.size());
  EXPECT_GE(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 2 * 15 - 5);
  EXPECT_EQ(module_->writes_while_busy.load(), 0);
}

TEST_P(E220ModemPolarityTest, TheOppositeLevelMeansBusyAndConfigureFailsWithAnAuxError) {
  cfg_.aux_idle_level = !GetParam();  // the modem expects the other level than the module idles at
  auto m = Make(cfg_);
  const auto r = m->Configure({});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("AUX"), std::string::npos);
}

INSTANTIATE_TEST_SUITE_P(IdleLevel, E220ModemPolarityTest, ::testing::Bool(),
                         [](const ::testing::TestParamInfo<bool>& i) { return i.param ? "High" : "Low"; });

// ---------------------------------------------------------------- Receive

TEST_F(E220ModemTest, ReceiveReturnsTheBytesFromTheUartAndAnEmptyVectorOnTimeout) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  auto none = m->Receive(milliseconds(10), {});
  ASSERT_TRUE(none.HasValue());
  EXPECT_TRUE(none.Value().empty());
  module_->InjectRx({1, 2, 3, 4});
  auto got = m->Receive(milliseconds(100), {});
  ASSERT_TRUE(got.HasValue());
  EXPECT_EQ(got.Value(), (std::vector<std::uint8_t>{1, 2, 3, 4}));
}

TEST_F(E220ModemTest, ReceiveReturnsPromptlyOnStop) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  std::stop_source src;
  src.request_stop();
  const auto t0 = Clock::now();
  const auto r = m->Receive(milliseconds(2000), src.get_token());
  EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 500);
  ASSERT_TRUE(r.HasValue());
  EXPECT_TRUE(r.Value().empty());
}

TEST_F(E220ModemTest, UartReadFailureIsPersistent) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  module_->uart_read_fails = true;
  module_->InjectRx({1, 2, 3});
  const auto r = m->Receive(milliseconds(100), {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
}

TEST_F(E220ModemTest, CloseClosesTheUartAndIsIdempotent) {
  auto m = Make();
  ASSERT_TRUE(m->Configure({}).HasValue());
  EXPECT_TRUE(module_->uart_open());
  m->Close();
  EXPECT_FALSE(module_->uart_open());
  m->Close();
}

TEST_F(E220ModemTest, ConfigureAfterCloseWorksAgain) {
  E220Config cfg;
  cfg.channel = 30;
  auto m = Make(cfg);
  ASSERT_TRUE(m->Configure({}).HasValue());
  m->Close();
  const int writes = module_->eeprom_writes.load();
  ASSERT_TRUE(m->Configure({}).HasValue());  // the second configuration: no new register writes
  EXPECT_EQ(module_->eeprom_writes.load(), writes);
  EXPECT_TRUE(m->Transmit(std::vector<std::uint8_t>(10, 9), {}).HasValue());
}

}  // namespace
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
