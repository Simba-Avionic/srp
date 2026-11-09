/**
 * @copyright Copyright (c) 2025
 */
// Audit tests (bugs): assert the CORRECT behaviour, RED on the current code.
#include <gtest/gtest.h>
#include <fcntl.h>
#include <pty.h>
#include <unistd.h>
#include <string>
#include "core/uart/uart_driver.hpp"

/**
 * F-46 (srednia) core/uart/uart_driver.cpp:51-83. Open() keeps the canonical/cooked line discipline (ICANON, ICRNL,
 * IXON/IXOFF, ECHO, OPOST - the code that clears them is commented out). Binary frames (rs485/modbus) are altered:
 * 0x0D becomes 0x0A, 0x11/0x13 (XON/XOFF) are swallowed. Expected: bytes read == bytes sent. Actual: they differ.
 */
TEST(AuditBugs, F46_UartOpenLeavesCookedMode) {
  auto child = [] {
        alarm(20);
        int master = -1, slave = -1;
        char name[128];
        if (openpty(&master, &slave, name, nullptr, nullptr) != 0) _exit(3);
        srp::core::uart::UartDriver u;
        if (!u.Open(name, B9600)) _exit(4);
        const std::vector<uint8_t> frame = {0x41, 0x0D, 0x42, 0x11, 0x43, 0x13, 0x44, 0x0A};  // 0x0A ends the line
        if (write(master, frame.data(), frame.size()) < 0) _exit(5);
        auto r = u.Read(16);
        _exit(r.has_value() && *r == frame ? 0 : 6);
      };
  EXPECT_EXIT(child(), ::testing::ExitedWithCode(0), "")
      << "F-46: UartDriver::Open leaves ICRNL/IXON/ICANON on, binary frame altered (uart_driver.cpp:51-83)";
}
