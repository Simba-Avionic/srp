/**
 * @file rs485_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <gtest/gtest.h>
#include "core/pd-33x/rs485/rs485.hpp"
#include "mw/gpio_server/controller/mock/mock_gpio_controller.h"
#include "core/uart/mock_uart_driver.hpp"

using ::testing::Return;
using ::testing::_;
using srp::core::RS485_conf_t;


TEST(RS485Test, InitSuccess) {
    srp::core::RS485 rs485;
    RS485_conf_t config{1, "COM1", 9600};
    auto uart_mock = std::make_unique<MockUartDriver>();
    auto gpio_mock = std::make_unique<srp::mock::MOCKGPIOController>();

    EXPECT_CALL(*uart_mock, Open(_, _)).WillOnce(Return(true));
    EXPECT_CALL(*uart_mock, Close()).Times(1);

    EXPECT_TRUE(rs485.Init(config, std::move(uart_mock), std::move(gpio_mock)));
}

TEST(RS485Test, InitFailure) {
    srp::core::RS485 rs485;
    RS485_conf_t config{1, "COM1", 9600};
    auto uart_mock = std::make_unique<MockUartDriver>();
    auto gpio_mock = std::make_unique<srp::mock::MOCKGPIOController>();

    EXPECT_CALL(*uart_mock, Open(_, _)).WillOnce(Return(false));
    EXPECT_CALL(*uart_mock, Close()).Times(1);

    EXPECT_FALSE(rs485.Init(config, std::move(uart_mock), std::move(gpio_mock)));
}

TEST(RS485Test, WriteReadSuccess) {
    srp::core::RS485 rs485;
    RS485_conf_t config{1, "COM1", 9600};
    std::vector<uint8_t> data = {0x01, 0x02};
    std::vector<uint8_t> response = {0x03, 0x04};

    auto uart_mock = std::make_unique<MockUartDriver>();
    auto gpio_mock = std::make_unique<srp::mock::MOCKGPIOController>();

    EXPECT_CALL(*uart_mock, Open(_, _)).WillOnce(Return(true));
    EXPECT_CALL(*gpio_mock, SetPinValue(1, 1)).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*uart_mock, Write(data)).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*gpio_mock, SetPinValue(1, 0)).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*uart_mock, Read(2)).WillOnce(Return(response));
    EXPECT_CALL(*uart_mock, Close()).Times(1);

    ASSERT_TRUE(rs485.Init(config, std::move(uart_mock), std::move(gpio_mock)));
    auto result = rs485.WriteRead(data, 2);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), response);
}

TEST(RS485Test, WriteReadFail1) {
    srp::core::RS485 rs485;
    RS485_conf_t config{1, "COM1", 9600};
    std::vector<uint8_t> data = {0x01, 0x02};
    std::vector<uint8_t> response = {0x03, 0x04};

    auto uart_mock = std::make_unique<MockUartDriver>();
    auto gpio_mock = std::make_unique<srp::mock::MOCKGPIOController>();

    EXPECT_CALL(*uart_mock, Open(_, _)).WillOnce(Return(true));
    EXPECT_CALL(*gpio_mock, SetPinValue(1, 1)).WillOnce(Return(srp::core::ErrorCode::kNotDefine));
    EXPECT_CALL(*uart_mock, Close()).Times(1);

    ASSERT_TRUE(rs485.Init(config, std::move(uart_mock), std::move(gpio_mock)));
    auto result = rs485.WriteRead(data, 2);
    ASSERT_FALSE(result.has_value());
}

TEST(RS485Test, WriteReadFail2) {
    srp::core::RS485 rs485;
    RS485_conf_t config{1, "COM1", 9600};
    std::vector<uint8_t> data = {0x01, 0x02};
    std::vector<uint8_t> response = {0x03, 0x04};

    auto uart_mock = std::make_unique<MockUartDriver>();
    auto gpio_mock = std::make_unique<srp::mock::MOCKGPIOController>();

    EXPECT_CALL(*uart_mock, Open(_, _)).WillOnce(Return(true));
    EXPECT_CALL(*gpio_mock, SetPinValue(1, 1)).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*uart_mock, Write(data)).WillOnce(Return(srp::core::ErrorCode::kNotDefine));
    EXPECT_CALL(*uart_mock, Close()).Times(1);

    ASSERT_TRUE(rs485.Init(config, std::move(uart_mock), std::move(gpio_mock)));
    auto result = rs485.WriteRead(data, 2);
    ASSERT_FALSE(result.has_value());
}
TEST(RS485Test, WriteReadFail3) {
    srp::core::RS485 rs485;
    RS485_conf_t config{1, "COM1", 9600};
    std::vector<uint8_t> data = {0x01, 0x02};
    std::vector<uint8_t> response = {0x03, 0x04};

    auto uart_mock = std::make_unique<MockUartDriver>();
    auto gpio_mock = std::make_unique<srp::mock::MOCKGPIOController>();

    EXPECT_CALL(*uart_mock, Open(_, _)).WillOnce(Return(true));
    EXPECT_CALL(*gpio_mock, SetPinValue(1, 1)).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*uart_mock, Write(data)).WillOnce(Return(srp::core::ErrorCode::kOk));
    EXPECT_CALL(*gpio_mock, SetPinValue(1, 0)).WillOnce(Return(srp::core::ErrorCode::kNotDefine));
    EXPECT_CALL(*uart_mock, Close()).Times(1);

    ASSERT_TRUE(rs485.Init(config, std::move(uart_mock), std::move(gpio_mock)));
    auto result = rs485.WriteRead(data, 2);
    ASSERT_FALSE(result.has_value());
}
