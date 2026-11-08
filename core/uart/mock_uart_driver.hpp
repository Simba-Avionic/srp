/**
 * @file mock_uart_driver.hpp
 * @brief Mock IUartDriver (gmock).
 *
 * @copyright Copyright (c) 2025
 */
#ifndef CORE_UART_MOCK_UART_DRIVER_HPP_
#define CORE_UART_MOCK_UART_DRIVER_HPP_
#include <gmock/gmock.h>
#include <optional>
#include <vector>
#include <string>
#include "core/uart/Iuart_driver.hpp"

class MockUartDriver : public srp::core::uart::IUartDriver {
 public:
    MOCK_METHOD(bool, Open, (const std::string& portName, const uint32_t& baudrate, const uint8_t timeout), (override));
    MOCK_METHOD(bool, Open, (const std::string& portName, const srp::core::uart::UartConfig& config), (override));
    MOCK_METHOD(bool, WaitForData, (int timeout_ms), (override));
    MOCK_METHOD(std::optional<std::vector<uint8_t>>, Read, (const uint16_t size), (override));
    MOCK_METHOD(void, Close, (), (override));
    MOCK_METHOD(srp::core::ErrorCode, Write, (const std::vector<uint8_t>& data), (override));
};

#endif  // CORE_UART_MOCK_UART_DRIVER_HPP_
