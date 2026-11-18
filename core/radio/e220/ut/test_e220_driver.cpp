/**
 * Copyright (c) 2024
 *
 * @file test_e220_driver.cpp
 * @brief E220RadioDriver na podstawce modulu: sekwencja Start(), idempotencja, AUX, Send/OnTxReady (bufor 400 B).
 */
#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <chrono>  // NOLINT
#include <memory>
#include <thread>  // NOLINT
#include <tuple>
#include <vector>

#include "ara/com/mavlink/transport_error.h"
#include "core/radio/e220/e220_radio_driver.hpp"
#include "core/radio/e220/testing/fake_e220_module.hpp"
#include "core/radio/e220/testing/recording_listener.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
using ara::com::mavlink::IsTransportError;
using ara::com::mavlink::TransportErrc;
using testing::FakeE220Module;
using testing::RecordingListener;
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
  t.aux_fall_wait = milliseconds(2);
  t.read_slice = milliseconds(20);
  return t;
}

E220Config FastAir() {
  E220Config c;
  c.air_rate_bps = 62500;  // szybki drenaz modelu bufora: testy Write nie czekaja sekundami
  return c;
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

// ---------------------------------------------------------------- Start: konfiguracja

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
  // sekwencja: odczyt, 3 zapisy, odczyt weryfikujacy
  ASSERT_EQ(module_->commands.size(), 5U);
  EXPECT_EQ(module_->commands[0], (std::vector<std::uint8_t>{0xC1, 0x00, 0x06}));
  EXPECT_EQ(module_->commands[1], (std::vector<std::uint8_t>{0xC0, 0x02, 0x01, 0xC2}));
  EXPECT_EQ(module_->commands[2], (std::vector<std::uint8_t>{0xC0, 0x03, 0x01, 0x01}));
  EXPECT_EQ(module_->commands[3], (std::vector<std::uint8_t>{0xC0, 0x04, 0x01, 0x12}));
  EXPECT_EQ(module_->commands[4], (std::vector<std::uint8_t>{0xC1, 0x00, 0x06}));
}

TEST_F(E220Test, ReadWithoutDifferencesWritesZeroBytesC0) {
  auto t = Make();  // domyslna konfiguracja = ustawienia fabryczne modulu
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(CountCommands(0xC0), 0U);
  EXPECT_EQ(module_->eeprom_writes.load(), 0);
  EXPECT_EQ(CountCommands(0xC1), 1U);  // tylko odczyt, bez weryfikacji (nic nie zapisano)
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
  EXPECT_EQ(module_->eeprom_writes.load(), writes_after_first);  // 0 nowych zapisow
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
  module_->eeprom[kReg3] = 0xC3;  // bajt RSSI + transmisja fixed (zepsulaby MAVLink)
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->written_registers, std::vector<int>{kReg3});
  EXPECT_EQ(module_->RegistersNow()[kReg3], 0x03);
}

TEST_F(E220Test, Reg1AmbientRssiAndSoftSwitchAreCleared) {
  module_->eeprom[kReg1] = 0x24;
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->written_registers, std::vector<int>{kReg1});
  EXPECT_EQ(module_->RegistersNow()[kReg1], 0x00);
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
    EXPECT_LE(c[1] + (c[0] == 0xC1 ? c[2] : 1), 6);  // zakres 00H-05H
  }
}

TEST_F(E220Test, LastRegistersReflectTheModuleAfterStart) {
  E220Config cfg;
  cfg.channel = 7;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(t->last_registers()[kReg2], 7);
}

// ---------------------------------------------------------------- tryby, UART, baud

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
  EXPECT_EQ(module_->uart_opens[1].baud, B57600);  // ponowne otwarcie po zmianie REG0
}

TEST_F(E220Test, Baud9600DoesNotReopenTheUart) {
  auto t = Make();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  EXPECT_EQ(module_->uart_opens.size(), 1U);
}

TEST_F(E220Test, ParityFromConfigIsAppliedWhenReopening) {
  E220Config cfg;
  cfg.parity = UartParity::k8E1;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  ASSERT_EQ(module_->uart_opens.size(), 2U);
  EXPECT_EQ(module_->uart_opens[1].parity, uart::Parity::kEven);
  EXPECT_EQ(module_->uart_opens[1].baud, B9600);
}

// ---------------------------------------------------------------- bledy i retry

TEST_F(E220Test, VerificationMismatchRetriesThreeTimesThenRetryableError) {
  module_->writes_do_not_stick = true;
  E220Config cfg;
  cfg.channel = 44;
  auto t = Make(cfg);
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
  EXPECT_NE(r.Error().SupportData().find("weryfikacja"), std::string::npos);
  EXPECT_NE(r.Error().SupportData().find("3 probach"), std::string::npos);
  EXPECT_EQ(module_->read_commands.load(), 6);  // odczyt + weryfikacja x 3 proby
  EXPECT_EQ(module_->eeprom_writes.load(), 3);
  EXPECT_EQ(module_->mode_log.back(), (std::pair<bool, bool>{false, false}));  // modul wraca do trybu normalnego
}

TEST_F(E220Test, AuxStuckLowWaitsWithTimeoutAndFails) {
  module_->aux_forced_low = true;
  auto t = Make();
  const auto t0 = Clock::now();
  const auto r = t->Start(lis_, {});
  const auto ms = std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count();
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kRetryable));
  EXPECT_NE(r.Error().SupportData().find("AUX"), std::string::npos);
  EXPECT_GE(ms, 3 * 100 - 20);  // 3 proby po timeout AUX = 100 ms
  EXPECT_EQ(module_->commands.size(), 0U);  // nic nie wyslano do zajetego modulu
}

TEST_F(E220Test, AuxReleasedLateIsWaitedFor) {
  module_->aux_forced_low = true;
  std::thread release{[this] {
    std::this_thread::sleep_for(milliseconds(40));
    module_->aux_forced_low = false;
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
  EXPECT_NE(r.Error().SupportData().find("brak odpowiedzi"), std::string::npos);
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
  module_->aux_forced_low = true;
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
  module_->aux_forced_low = true;
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

TEST_F(E220Test, SendDeliversWholeFrameToModuleInOneUartWrite) {
  auto t = Make(FastAir());
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const std::vector<std::uint8_t> frame(100, 0x5A);
  ASSERT_TRUE(t->Send(frame).HasValue());
  std::vector<std::uint8_t> got;
  for (int i = 0; i < 200 && got.size() < frame.size(); ++i) {
    const auto part = module_->DrainTx();
    got.insert(got.end(), part.begin(), part.end());
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_EQ(got, frame);
  ASSERT_EQ(module_->data_writes.size(), 1U);  // cala ramka jednym zapisem UART
  t->Stop();
}

TEST_F(E220Test, SendNeverBlocksEvenWhenAuxIsBusy) {
  module_->aux_busy_ms = 30;  // AUX nisko po kazdym zapisie
  auto t = Make(FastAir());
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  for (int i = 0; i < 6; ++i) {
    const auto t0 = Clock::now();
    const auto r = t->Send(std::vector<std::uint8_t>(50, static_cast<std::uint8_t>(i)));
    EXPECT_TRUE(r.HasValue() || IsTransportError(r.Error(), TransportErrc::kBusy));
    EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 20);
  }
  t->Stop();
}

TEST_F(E220Test, BusyWhenModuleBufferModelIsFullThenTxReadyOnce) {
  E220Config cfg;
  cfg.air_rate_bps = 9600;  // ~1200 B/s * 0.8: 200 B schodzi z bufora w ok. 200 ms
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const std::vector<std::uint8_t> frame(200, 1);
  ASSERT_TRUE(t->Send(frame).HasValue());
  ASSERT_TRUE(t->Send(frame).HasValue());  // 2 x 200 B = 400 B: bufor modulu pelny
  const auto busy = t->Send(frame);
  ASSERT_FALSE(busy.HasValue());
  EXPECT_TRUE(IsTransportError(busy.Error(), TransportErrc::kBusy));
  EXPECT_EQ(lis_.tx_ready(), 0);
  // miejsce zwalnia sie po czasie antenowym pakietu: OnTxReady (raz), potem Send znowu przyjmuje ramke
  const auto end = Clock::now() + milliseconds(3000);
  while (lis_.tx_ready() == 0 && Clock::now() < end) {
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_EQ(lis_.tx_ready(), 1);
  std::this_thread::sleep_for(milliseconds(100));
  EXPECT_EQ(lis_.tx_ready(), 1);  // tylko jeden sygnal na kBusy
  EXPECT_TRUE(t->Send(frame).HasValue());
  t->Stop();
}

TEST_F(E220Test, TxReadyAlsoWhenBusyWasReturnedJustBeforeRoomAppeared) {
  E220Config cfg;
  cfg.air_rate_bps = 62500;  // miejsce zwalnia sie w ulamku sekundy
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const std::vector<std::uint8_t> frame(200, 1);
  int busy = 0;
  for (int i = 0; i < 40; ++i) {
    const auto r = t->Send(frame);
    if (!r.HasValue()) {
      ++busy;
      const int before = lis_.tx_ready();
      const auto end = Clock::now() + milliseconds(2000);
      while (lis_.tx_ready() == before && Clock::now() < end) {
        std::this_thread::sleep_for(milliseconds(2));
      }
      EXPECT_GT(lis_.tx_ready(), before) << "brak OnTxReady po kBusy (nr " << i << ")";
    }
  }
  EXPECT_GT(busy, 0);
  t->Stop();
}

TEST_F(E220Test, FrameLargerThanModuleBufferIsInvalidNotBusy) {
  auto t = Make(FastAir());
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const auto r = t->Send(std::vector<std::uint8_t>(401, 0));
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kInvalidArgument));
  t->Stop();
}

TEST_F(E220Test, EstimatedModuleOccupancyNeverExceeds400Bytes) {
  // seria ramek jedna po drugiej: zadna porcja nie moze trafic do modulu, gdy model pokazuje < 400 B wolnego
  E220Config cfg;
  cfg.air_rate_bps = 9600;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  std::size_t sent_bytes = 0;
  const auto end = Clock::now() + milliseconds(1500);
  while (Clock::now() < end) {
    const std::vector<std::uint8_t> frame(150, 7);
    if (t->Send(frame).HasValue()) {
      sent_bytes += frame.size();
    } else {
      std::this_thread::sleep_for(milliseconds(5));
    }
    const double air_s = static_cast<double>(std::chrono::duration_cast<milliseconds>(end - Clock::now()).count());
    static_cast<void>(air_s);
  }
  // 9600 bps * 0.8 = 960 B/s; w 1.5 s (+ 400 B bufora) nie wiecej niz ~1850 B
  EXPECT_LE(sent_bytes, 400 + 960 * 1.5 + 150);
  EXPECT_GT(sent_bytes, 400U);
  t->Stop();
}

TEST_F(E220Test, AuxIsAwaitedBeforeEachUartWrite) {
  module_->aux_busy_ms = 25;  // po kazdym zapisie AUX nisko 25 ms
  auto t = Make(FastAir());
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const auto t0 = Clock::now();
  for (int i = 0; i < 3; ++i) {
    ASSERT_TRUE(t->Send(std::vector<std::uint8_t>(40, static_cast<std::uint8_t>(i))).HasValue());
  }
  while (module_->data_writes.size() < 3 && Clock::now() - t0 < milliseconds(2000)) {
    std::this_thread::sleep_for(milliseconds(2));
  }
  ASSERT_EQ(module_->data_writes.size(), 3U);
  EXPECT_GE(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 2 * 25 - 10);  // 2 przerwy AUX
  t->Stop();
}

TEST_F(E220Test, AuxStuckLowAfterStartIsFatalAndSendFailsAfterwards) {
  E220Timing t_aux = FastTiming();
  t_aux.aux_timeout = milliseconds(80);
  auto t = Make(FastAir(), t_aux);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  module_->aux_forced_low = true;
  ASSERT_TRUE(t->Send(std::vector<std::uint8_t>(20, 1)).HasValue());  // przyjeta do kolejki
  const auto end = Clock::now() + milliseconds(2000);
  while (lis_.fatal_errors() == 0 && Clock::now() < end) {
    std::this_thread::sleep_for(milliseconds(5));
  }
  EXPECT_EQ(lis_.fatal_errors(), 1);
  EXPECT_TRUE(module_->data_writes.empty());  // nic nie trafilo do zajetego modulu
  const auto r = t->Send(std::vector<std::uint8_t>(20, 1));
  ASSERT_FALSE(r.HasValue());
  EXPECT_FALSE(IsTransportError(r.Error(), TransportErrc::kBusy));
  t->Stop();
}

TEST_F(E220Test, UartWriteFailureIsFatal) {
  auto t = Make(FastAir());
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

TEST_F(E220Test, ReceivedBytesReachListenerAsChunksInOrder) {
  auto t = Make(FastAir());
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
  auto t = Make(FastAir());
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
  auto t = Make(FastAir());
  EXPECT_TRUE(IsTransportError(t->Send(std::vector<std::uint8_t>(5, 0)).Error(), TransportErrc::kClosed));
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  t->Stop();
  EXPECT_TRUE(IsTransportError(t->Send(std::vector<std::uint8_t>(5, 0)).Error(), TransportErrc::kClosed));
  t->Stop();  // idempotentne
}

TEST_F(E220Test, StartAfterStopReconfiguresAndWorks) {
  E220Config cfg = FastAir();
  cfg.channel = 30;
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  t->Stop();
  const int writes = module_->eeprom_writes.load();
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());  // drugi start: 0 nowych zapisow rejestrow
  EXPECT_EQ(module_->eeprom_writes.load(), writes);
  ASSERT_TRUE(t->Send(std::vector<std::uint8_t>(10, 9)).HasValue());
  t->Stop();
}

TEST_F(E220Test, StartWithoutStopIsInvalid) {
  auto t = Make(FastAir());
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const auto r = t->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kInvalidArgument));
  t->Stop();
}

TEST_F(E220Test, StopWithQueuedFramesLeavesNoCallbacks) {
  module_->aux_busy_ms = 40;
  auto t = Make(FastAir());
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
  E220Config cfg;
  cfg.air_rate_bps = 2400;  // 200 B schodzi z bufora modelu ok. 0,8 s: Stop() trafia w oczekiwanie na OnTxReady
  auto t = Make(cfg);
  ASSERT_TRUE(t->Start(lis_, {}).HasValue());
  const std::vector<std::uint8_t> frame(200, 1);
  ASSERT_TRUE(t->Send(frame).HasValue());
  ASSERT_TRUE(t->Send(frame).HasValue());  // 2 x 200 B = 400 B: bufor modulu pelny
  const auto busy = t->Send(frame);
  ASSERT_FALSE(busy.HasValue());
  ASSERT_TRUE(IsTransportError(busy.Error(), TransportErrc::kBusy));
  std::this_thread::sleep_for(milliseconds(50));
  ASSERT_EQ(lis_.tx_ready(), 0);  // watek TX czeka na zwolnienie miejsca (OnTxReady jeszcze nie padl)
  const auto t0 = Clock::now();
  t->Stop();
  EXPECT_LT(std::chrono::duration_cast<milliseconds>(Clock::now() - t0).count(), 1000)
      << "Stop() zawisl na oczekiwaniu na OnTxReady";
  lis_.Seal();
  std::this_thread::sleep_for(milliseconds(1200));  // dluzej niz czas zwolnienia miejsca w modelu
  EXPECT_EQ(lis_.tx_ready(), 0);
  EXPECT_EQ(lis_.violations(), 0) << "callback po powrocie ze Stop() w trakcie oczekiwania na OnTxReady";
  const auto after = t->Send(frame);
  ASSERT_FALSE(after.HasValue());
  EXPECT_TRUE(IsTransportError(after.Error(), TransportErrc::kClosed));
}

TEST_F(E220Test, ConfigErrorDriverFailsStartPersistentlyWithTheMessage) {
  auto d = E220RadioDriver::WithConfigError("ebyte_e220", "plik '/x.json': brak pola 'uart'");
  const auto r = d->Start(lis_, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  EXPECT_NE(r.Error().SupportData().find("brak pola 'uart'"), std::string::npos);
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
