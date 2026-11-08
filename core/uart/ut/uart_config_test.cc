/**
 * @file uart_config_test.cc
 * @brief Dowod zachowania 1:1: UartConfig::Legacy()/Mavlink() daja te same flagi termios co dawny core/uart
 *        i dawny radio-driver (kod referencyjny skopiowany z 908a071).
 *
 * @copyright Copyright (c) 2025
 */
#include <gtest/gtest.h>
#include <pty.h>
#include <termios.h>
#include <unistd.h>

#include <cstring>
#include <vector>

#include "core/uart/mock_uart_driver.hpp"
#include "core/uart/uart_config.hpp"

using srp::core::uart::GetBaudrateConstant;
using srp::core::uart::UartConfig;

namespace {

// Dawny core/uart/uart_driver.cpp, Open() (po tcgetattr, flagi 70-77 byly zakomentowane).
void OldCoreUart(struct termios* tty, speed_t baudrate) {
  cfsetospeed(tty, baudrate);
  cfsetispeed(tty, baudrate);
  tty->c_cflag &= ~PARENB;
  tty->c_cflag &= ~CSTOPB;
  tty->c_cflag &= ~CSIZE;
  tty->c_cflag |= CS8;
  tty->c_cflag &= ~CRTSCTS;
  tty->c_cflag |= CREAD | CLOCAL;
  tty->c_cc[VTIME] = 10;
  tty->c_cc[VMIN] = 0;
}

// Dawny apps/{ec,fc}/radio_service/mavlink_uart_driver.cpp, Open() (po tcgetattr).
void OldMavUart(struct termios* tty, speed_t baudrate) {
  tty->c_cflag &= ~PARENB;
  tty->c_cflag &= ~CSTOPB;
  tty->c_cflag &= ~CSIZE;
  tty->c_cflag |= CS8;
  tty->c_cflag &= ~CRTSCTS;
  tty->c_lflag &= ~ICANON;
  tty->c_lflag &= ~ISIG;
  tty->c_lflag &= ~ECHO;
  tty->c_lflag &= ~ECHOE;
  tty->c_lflag &= ~ECHONL;
  tty->c_iflag &= ~(IXON | IXOFF | IXANY);
  tty->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
  tty->c_oflag &= ~OPOST;
  tty->c_oflag &= ~ONLCR;
  tty->c_cflag |= CREAD | CLOCAL;
  tty->c_cc[VTIME] = 10;
  tty->c_cc[VMIN] = 0;
  cfsetospeed(tty, baudrate);
  cfsetispeed(tty, baudrate);
}

void ExpectSame(const struct termios& a, const struct termios& b, const char* what) {
  EXPECT_EQ(a.c_iflag, b.c_iflag) << what << " c_iflag";
  EXPECT_EQ(a.c_oflag, b.c_oflag) << what << " c_oflag";
  EXPECT_EQ(a.c_cflag, b.c_cflag) << what << " c_cflag";
  EXPECT_EQ(a.c_lflag, b.c_lflag) << what << " c_lflag";
  EXPECT_EQ(a.c_line, b.c_line) << what << " c_line";
  for (int i = 0; i < NCCS; ++i) {
    EXPECT_EQ(a.c_cc[i], b.c_cc[i]) << what << " c_cc[" << i << "]";
  }
  EXPECT_EQ(cfgetospeed(&a), cfgetospeed(&b)) << what << " ospeed";
  EXPECT_EQ(cfgetispeed(&a), cfgetispeed(&b)) << what << " ispeed";
}

// Stany poczatkowe: same zera, same jedynki i prawdziwy tty (pty) z domyslnym termios.
std::vector<struct termios> InitialStates() {
  std::vector<struct termios> v;
  struct termios z;
  std::memset(&z, 0, sizeof(z));
  v.push_back(z);
  struct termios ones;
  std::memset(&ones, 0xFF, sizeof(ones));
  v.push_back(ones);
  int master = -1;
  int slave = -1;
  if (openpty(&master, &slave, nullptr, nullptr, nullptr) == 0) {
    struct termios real;
    if (tcgetattr(slave, &real) == 0) {
      v.push_back(real);
    }
    close(master);
    close(slave);
  }
  return v;
}

}  // namespace

TEST(UartConfigTest, LegacyEqualsOldCoreUart) {
  for (speed_t baud : {B9600, B57600, B115200}) {
    for (auto init : InitialStates()) {
      struct termios expected = init;
      struct termios actual = init;
      OldCoreUart(&expected, baud);
      UartConfig::Legacy(baud).ApplyTo(&actual);
      ExpectSame(expected, actual, "Legacy");
    }
  }
}

TEST(UartConfigTest, MavlinkEqualsOldRadioDriver) {
  for (uint32_t baud : {57600u, 115200u, 921600u}) {
    for (auto init : InitialStates()) {
      struct termios expected = init;
      struct termios actual = init;
      OldMavUart(&expected, GetBaudrateConstant(baud));
      UartConfig::Mavlink(baud).ApplyTo(&actual);
      ExpectSame(expected, actual, "Mavlink");
    }
  }
}

TEST(UartConfigTest, PresetsKeepDriverLevelBehaviour) {
  const auto legacy = UartConfig::Legacy(B9600);
  EXPECT_FALSE(legacy.raw);
  EXPECT_EQ(legacy.default_read_size, 256);      // dawny Read(0): do 256 B
  EXPECT_FALSE(legacy.fail_on_apply_error);      // dawny Open() ignorowal wynik tcsetattr()
  const auto mav = UartConfig::Mavlink(57600);
  EXPECT_TRUE(mav.raw);
  EXPECT_EQ(mav.default_read_size, 512);         // dawny radio-driver::Read(0): do 512 B
  EXPECT_TRUE(mav.fail_on_apply_error);          // dawny Open() zwracal wynik tcsetattr()
  EXPECT_EQ(mav.baudrate, static_cast<speed_t>(B57600));
}

TEST(UartConfigTest, BaudrateTable) {
  EXPECT_EQ(GetBaudrateConstant(57600), static_cast<speed_t>(B57600));
  EXPECT_EQ(GetBaudrateConstant(115200), static_cast<speed_t>(B115200));
  EXPECT_EQ(GetBaudrateConstant(12345), static_cast<speed_t>(0));
}

TEST(UartConfigTest, CustomFormat7E2) {
  UartConfig c = UartConfig::Legacy(B9600);
  c.data_bits = 7;
  c.parity = srp::core::uart::Parity::kEven;
  c.stop_bits = 2;
  c.hw_flow_control = true;
  struct termios t;
  std::memset(&t, 0, sizeof(t));
  c.ApplyTo(&t);
  EXPECT_EQ(t.c_cflag & CSIZE, static_cast<tcflag_t>(CS7));
  EXPECT_TRUE(t.c_cflag & PARENB);
  EXPECT_FALSE(t.c_cflag & PARODD);
  EXPECT_TRUE(t.c_cflag & CSTOPB);
  EXPECT_TRUE(t.c_cflag & CRTSCTS);
}

TEST(UartConfigTest, MockAcceptsConfigOpenAndWaitForData) {
  MockUartDriver mock;
  EXPECT_CALL(mock, Open(::testing::_, ::testing::An<const UartConfig&>())).WillOnce(::testing::Return(true));
  EXPECT_CALL(mock, WaitForData(250)).WillOnce(::testing::Return(true));
  srp::core::uart::IUartDriver& drv = mock;
  EXPECT_TRUE(drv.Open("/dev/ttyS1", UartConfig::Mavlink(57600)));
  EXPECT_TRUE(drv.WaitForData(250));
}
