/**
 * @copyright Copyright (c) 2025
 */
// Regression test: F-34 was found by the audit on 8813de3 and is fixed on the current base (UartDriver::Read
// reads into a heap vector of the requested size).
#include <gtest/gtest.h>
#include <fcntl.h>
#include <pty.h>
#include <unistd.h>
#include <string>
#include "core/uart/uart_driver.hpp"

/**
 * F-34 (srednia) core/uart/uart_driver.cpp:~76-80. Read(size) reads into uint8_t read_buf[256] on the stack
 * but passes size (uint16) to read(): Read(1000) with >256 B pending overflows the stack.
 * Expected: Read(1000) with 1000+ B pending returns at most 1000 B and the process survives (built with ASAN).
 * Run in a child (death test) so the verdict does not depend on how the overflow manifests.
 */
TEST(Regression, F34_UartReadLargerThanDefaultBufferIsSafe) {
  auto child = [] {
        int master = -1, slave = -1;
        char name[128];
        if (openpty(&master, &slave, name, nullptr, nullptr) != 0) _exit(3);
        srp::core::uart::UartDriver u;
        if (!u.Open(name, B9600)) _exit(4);
        std::string data(1000, 65);
        data += "\n";  // canonical mode: read() returns once a line is complete
        if (write(master, data.data(), data.size()) < 0) _exit(5);
        auto r = u.Read(1000);
        _exit(r.has_value() && r->size() <= 1000 ? 0 : 6);
      };
  EXPECT_EXIT(child(),
      ::testing::ExitedWithCode(0), "")
      << "F-34 regression: UartDriver::Read(1000) crashed or returned too much (uart_driver.cpp)";
}

