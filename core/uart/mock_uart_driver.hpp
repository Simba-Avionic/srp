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
    MOCK_METHOD(bool, Open, (const std::string& portName, const speed_t& baudrate), (override));
    MOCK_METHOD(std::optional<std::vector<char>>, Read, (const uint16_t size), (override));
    MOCK_METHOD(void, Close, (), (override));
};

#endif  // CORE_UART_MOCK_UART_DRIVER_HPP_
