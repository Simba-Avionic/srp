/**
 * @file uart_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "core/uart/uart_driver.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace srp::core::uart;  // NOLINT
using ::testing::Return;
using ::testing::_;
TEST(UartDriverTest, OpenFailure) {
    srp::core::uart::UartDriver uart;
    EXPECT_FALSE(uart.Open("/dev/invalid", B9600));
}

TEST(UartDriverTest, ReadFailure) {
    srp::core::uart::UartDriver uart;
    uart.Open("/dev/ttyUSB0", B9600);
    auto data = uart.Read(0);
    ASSERT_FALSE(data.has_value());
}

TEST(UartDriverTest, Close) {
    srp::core::uart::UartDriver uart;
    uart.Open("/dev/ttyUSB0", B9600);
    uart.Close();
}
