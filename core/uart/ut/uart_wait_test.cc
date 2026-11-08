/**
 * @file uart_wait_test.cc
 * @brief IUartDriver::WaitForData(timeout, stop_token): semantyka jak core::condition::wait_for.
 *        Prawdziwy driver na pty (a-d) oraz model sim (e), ktory jest testowany bezposrednio (UartSim).
 */
#include <gtest/gtest.h>
#include <fcntl.h>
#include <pty.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>  // NOLINT
#include <stop_token>  // NOLINT
#include <thread>  // NOLINT
#include "core/sim/sim_uart.hpp"
#include "core/uart/uart_driver.hpp"

using std::chrono::milliseconds;
using Clock = std::chrono::steady_clock;

namespace {
int64_t MsSince(Clock::time_point t) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t).count();
}
}  // namespace

#ifndef SRP_SIM_BUILD
class UartWaitTest : public ::testing::Test {
 protected:
  void SetUp() override {
    char name[128];
    ASSERT_EQ(openpty(&master_, &slave_, name, nullptr, nullptr), 0);
    ASSERT_TRUE(uart_.Open(name, srp::core::uart::UartConfig::Mavlink(57600)));
  }
  void TearDown() override {
    uart_.Close();
    close(master_);
    close(slave_);
  }
  int master_{-1}, slave_{-1};
  srp::core::uart::UartDriver uart_;
};

// (a) dane w trakcie czekania -> true szybciej niz timeout
TEST_F(UartWaitTest, DataArrivesDuringWaitReturnsTrue) {
  std::stop_source src;
  std::thread w([this] {
    std::this_thread::sleep_for(milliseconds(200));
    const char b = 0x78;
    ASSERT_EQ(write(master_, &b, 1), 1);
  });
  const auto t0 = Clock::now();
  EXPECT_TRUE(uart_.WaitForData(milliseconds(2000), src.get_token()));
  const auto dt = MsSince(t0);
  w.join();
  EXPECT_GE(dt, 150);
  EXPECT_LT(dt, 1000) << "data should wake the wait early";
}

// (b) brak danych -> false po ~timeout
TEST_F(UartWaitTest, NoDataReturnsFalseAfterTimeout) {
  std::stop_source src;
  const auto t0 = Clock::now();
  EXPECT_FALSE(uart_.WaitForData(milliseconds(300), src.get_token()));
  const auto dt = MsSince(t0);
  EXPECT_GE(dt, 280);
  EXPECT_LT(dt, 600);
}

// (c) request_stop z innego watku w trakcie 2 s czekania -> false w < 50 ms od zadania
TEST_F(UartWaitTest, StopDuringWaitReturnsFalseImmediately) {
  std::stop_source src;
  Clock::time_point t_stop;
  std::thread s([&] {
    std::this_thread::sleep_for(milliseconds(300));
    t_stop = Clock::now();
    src.request_stop();
  });
  const bool r = uart_.WaitForData(milliseconds(2000), src.get_token());
  const auto after_stop = MsSince(t_stop);
  s.join();
  EXPECT_FALSE(r);
  EXPECT_LT(after_stop, 50);
}

// (d) stop zazadany przed wywolaniem -> false bez czekania (< 5 ms), nawet gdy sa dane
TEST_F(UartWaitTest, StopBeforeCallReturnsFalseWithoutWaiting) {
  const char b = 0x78;
  ASSERT_EQ(write(master_, &b, 1), 1);
  std::stop_source src;
  src.request_stop();
  const auto t0 = Clock::now();
  EXPECT_FALSE(uart_.WaitForData(milliseconds(2000), src.get_token()));
  EXPECT_LT(MsSince(t0), 5);
}

// brak wycieku fd: wiele wywolan nie zwieksza liczby otwartych deskryptorow
TEST_F(UartWaitTest, NoFdLeak) {
  auto count = [] {
    int n = 0;
    for (int fd = 0; fd < 256; ++fd) {
      if (fcntl(fd, F_GETFD) != -1) {
        ++n;
      }
    }
    return n;
  };
  std::stop_source src;
  const int before = count();
  for (int i = 0; i < 50; ++i) {
    uart_.WaitForData(milliseconds(1), src.get_token());
  }
  EXPECT_EQ(count(), before);
}
#endif

// (e) sim: ta sama semantyka
class UartSimWaitTest : public ::testing::Test {
 protected:
  void SetUp() override {
    char tmpl[] = "/tmp/srpsim_waitXXXXXX";
    ASSERT_NE(mkdtemp(tmpl), nullptr);
    setenv("SRP_SIM_DIR", tmpl, 1);
    h_ = srp::core::sim::UartSim::Get().Open("/dev/ttyWAIT", 10);
    ASSERT_GE(h_, 0);
  }
  int h_{-1};
};

TEST_F(UartSimWaitTest, TimeoutStopDuringAndStopBefore) {
  auto& sim = srp::core::sim::UartSim::Get();
  std::stop_source none;
  auto t0 = Clock::now();
  EXPECT_FALSE(sim.WaitForData(h_, milliseconds(300), none.get_token()));
  EXPECT_GE(MsSince(t0), 280);
  EXPECT_LT(MsSince(t0), 600);

  std::stop_source src;
  Clock::time_point t_stop;
  std::thread s([&] {
    std::this_thread::sleep_for(milliseconds(300));
    t_stop = Clock::now();
    src.request_stop();
  });
  EXPECT_FALSE(sim.WaitForData(h_, milliseconds(2000), src.get_token()));
  EXPECT_LT(MsSince(t_stop), 50);
  s.join();

  t0 = Clock::now();
  EXPECT_FALSE(sim.WaitForData(h_, milliseconds(2000), src.get_token()));  // src juz zatrzymany
  EXPECT_LT(MsSince(t0), 5);
}
