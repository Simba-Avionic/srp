/**
 * @file mock_uart_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2024
 * 
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
    MOCK_METHOD(std::optional<std::vector<uint8_t>>, Read, (const uint16_t size), (override));
    MOCK_METHOD(void, Close, (), (override));
    MOCK_METHOD(srp::core::ErrorCode, Write, (const std::vector<uint8_t>& data), (override));
};

#endif  // CORE_UART_MOCK_UART_DRIVER_HPP_
