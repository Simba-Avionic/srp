/**
 * Copyright (c) 2024
 *
 * @file test_e220_io.cpp
 * @brief SerialPortUart (pty) and SysfsE220Pins (a temporary directory instead of /sys/class/gpio):
 * the hardware adapters of the E220 plugin (no ara::log).
 */
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>  // NOLINT
#include <cstdlib>
#include <fstream>
#include <string>
#include <thread>  // NOLINT
#include <tuple>
#include <vector>

#include "core/radio/e220/sysfs_e220_pins.hpp"
#include "core/radio/e220/serial_port_uart.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
using std::chrono::milliseconds;

struct Pty {
  Pty() {
    master = ::posix_openpt(O_RDWR | O_NOCTTY);
    EXPECT_GE(master, 0);
    EXPECT_EQ(::grantpt(master), 0);
    EXPECT_EQ(::unlockpt(master), 0);
    path = ::ptsname(master);
    slave_keep = ::open(path.c_str(), O_RDWR | O_NOCTTY);  // slave open: the master gets no EIO before Open()
  }
  ~Pty() {
    if (slave_keep >= 0) ::close(slave_keep);
    if (master >= 0) ::close(master);
  }
  int master{-1};
  int slave_keep{-1};
  std::string path;
};

TEST(SerialPortUart, ReadsAndWritesRawBytesThroughAPty) {
  Pty pty;
  SerialPortUart u;
  ASSERT_TRUE(u.Open(pty.path, uart::UartConfig::Mavlink(57600)));
  const std::uint8_t out[] = {0x00, 0xFD, 0x0A, 0x0D, 0xFF};  // 0x0A/0x0D: raw, no conversion
  ASSERT_EQ(::write(pty.master, out, sizeof(out)), static_cast<ssize_t>(sizeof(out)));
  ASSERT_TRUE(u.WaitForData(milliseconds(1000), {}));
  const auto data = u.Read(64);
  ASSERT_TRUE(data.has_value());
  EXPECT_EQ(*data, std::vector<std::uint8_t>(out, out + sizeof(out)));

  const std::vector<std::uint8_t> tx(300, 0x5A);
  ASSERT_EQ(u.Write(tx), core::ErrorCode::kOk);
  std::vector<std::uint8_t> got;
  const auto end = std::chrono::steady_clock::now() + milliseconds(2000);
  std::uint8_t buf[512];
  while (got.size() < tx.size() && std::chrono::steady_clock::now() < end) {
    const ssize_t n = ::read(pty.master, buf, sizeof(buf));
    if (n > 0) {
      got.insert(got.end(), buf, buf + n);
    } else {
      std::this_thread::sleep_for(milliseconds(2));
    }
  }
  EXPECT_EQ(got, tx);
}

TEST(SerialPortUart, ReopensAndRefusesWhatThePlatformSerialPortCannotDo) {
  Pty pty;
  SerialPortUart u;
  for (int round = 0; round < 3; ++round) {
    ASSERT_TRUE(u.Open(pty.path, uart::UartConfig::Mavlink(19200)));  // reopening after Close() works
    u.Close();
  }
  auto even = uart::UartConfig::Mavlink(19200);
  even.parity = uart::Parity::kEven;
  EXPECT_FALSE(u.Open(pty.path, even));  // SerialPort is 8N1 only
  EXPECT_FALSE(u.Open("/dev/no_such_uart_device", uart::UartConfig::Mavlink(9600)));
  uart::UartConfig bad;
  bad.baudrate = B0;
  EXPECT_FALSE(u.Open(pty.path, bad));
}

TEST(SerialPortUart, WaitForDataTimesOutAndStopTokenWakesIt) {
  Pty pty;
  SerialPortUart u;
  ASSERT_TRUE(u.Open(pty.path, uart::UartConfig::Mavlink(9600)));
  auto t0 = std::chrono::steady_clock::now();
  EXPECT_FALSE(u.WaitForData(milliseconds(60), {}));
  EXPECT_GE(std::chrono::steady_clock::now() - t0, milliseconds(50));
  std::stop_source src;
  std::thread stopper{[&] {
    std::this_thread::sleep_for(milliseconds(50));
    src.request_stop();
  }};
  t0 = std::chrono::steady_clock::now();
  EXPECT_FALSE(u.WaitForData(milliseconds(5000), src.get_token()));
  stopper.join();
  EXPECT_LT(std::chrono::steady_clock::now() - t0, milliseconds(1000));
}

TEST(SerialPortUart, ClosedPortAndLostDeviceAreReportedAsErrors) {
  SerialPortUart u;
  EXPECT_FALSE(u.Read(8).has_value());
  EXPECT_EQ(u.Write({1, 2, 3}), core::ErrorCode::kConnectionError);
  Pty pty;
  ASSERT_TRUE(u.Open(pty.path, uart::UartConfig::Mavlink(9600)));
  ::close(pty.slave_keep);
  pty.slave_keep = -1;
  ::close(pty.master);  // the "device vanished": the slave gets HUP/EIO
  pty.master = -1;
  EXPECT_TRUE(u.WaitForData(milliseconds(1000), {}));  // POLLHUP wakes the wait
  EXPECT_FALSE(u.Read(8).has_value());                 // a read error = device lost
  u.Close();
  u.Close();  // idempotent
}

// ---------------------------------------------------------------- SysfsE220Pins

class SysfsPinsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    char tmpl[] = "/tmp/srp_e220_gpio_XXXXXX";
    root_ = ::mkdtemp(tmpl);
    for (const int pin : {66, 67, 69}) {
      const std::string d = root_ + "/gpio" + std::to_string(pin);
      ::mkdir(d.c_str(), 0755);
      std::ofstream(d + "/direction") << "in";
      std::ofstream(d + "/value") << "0";
    }
  }
  void TearDown() override {
    const std::string cmd = "rm -rf '" + root_ + "'";
    std::ignore = std::system(cmd.c_str());
  }
  std::string Read(int pin, const char* leaf) {
    std::ifstream in{root_ + "/gpio" + std::to_string(pin) + "/" + leaf};
    std::string v;
    in >> v;
    return v;
  }
  std::string root_;
};

TEST_F(SysfsPinsTest, PrepareSetsDirectionsAndModeWritesM0M1) {
  SysfsE220Pins pins{66, 67, 69, root_};
  std::string err;
  ASSERT_TRUE(pins.Prepare(&err)) << err;
  EXPECT_EQ(Read(66, "direction"), "out");
  EXPECT_EQ(Read(67, "direction"), "out");
  EXPECT_EQ(Read(69, "direction"), "in");
  ASSERT_TRUE(pins.SetMode(true, false));
  EXPECT_EQ(Read(66, "value"), "1");
  EXPECT_EQ(Read(67, "value"), "0");
  ASSERT_TRUE(pins.SetMode(true, true));
  EXPECT_EQ(Read(67, "value"), "1");
}

TEST_F(SysfsPinsTest, AuxIsReadFromValueAndGarbageIsAnError) {
  SysfsE220Pins pins{66, 67, 69, root_};
  bool high = false;
  std::ofstream(root_ + "/gpio69/value") << "1";
  ASSERT_TRUE(pins.ReadAux(&high));
  EXPECT_TRUE(high);
  std::ofstream(root_ + "/gpio69/value", std::ios::trunc) << "0";
  ASSERT_TRUE(pins.ReadAux(&high));
  EXPECT_FALSE(high);
  std::ofstream(root_ + "/gpio69/value", std::ios::trunc) << "2";
  EXPECT_FALSE(pins.ReadAux(&high));  // must not be taken for an idle module
  std::ofstream(root_ + "/gpio69/value", std::ios::trunc) << "";
  EXPECT_FALSE(pins.ReadAux(&high));
}

TEST_F(SysfsPinsTest, MissingPinIsNamedInThePrepareError) {
  SysfsE220Pins pins{66, 67, 70, root_};  // GPIO 70 does not exist (export creates nothing)
  std::string err;
  EXPECT_FALSE(pins.Prepare(&err));
  EXPECT_NE(err.find("GPIO 70"), std::string::npos) << err;
}

}  // namespace
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
