/**
 * @file pd33x_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include "core/pd_33x/pd33x.hpp"
#include "core/uart/mock_uart_driver.hpp"
#include "mw/gpio_server/controller/mock/mock_gpio_controller.h"

class Crc16Test : public ::testing::TestWithParam<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> {};

TEST_P(Crc16Test, ComputeCRC) {
    srp::core::RS485 rs485;
    auto [input, expected] = GetParam();
    auto res = rs485.add_CRC(input);
    ASSERT_EQ(res.size(), expected.size());
    EXPECT_EQ(res, expected);
}

INSTANTIATE_TEST_SUITE_P(
    CRC16Tests,
    Crc16Test,
    ::testing::Values(
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x00, 0x02, 0x00, 0x02},
            std::vector<uint8_t>{0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xCB}),
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x04, 0x3F, 0x75, 0xF0, 0x7B},
            std::vector<uint8_t>{0x01, 0x03, 0x04, 0x3F, 0x75, 0xF0, 0x7B, 0xE3, 0xDE}),
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x01, 0x00, 0x00, 0x04},
            std::vector<uint8_t>{0x01, 0x03, 0x01, 0x00, 0x00, 0x04, 0x45, 0xF5}),
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x08, 0x3F, 0x75, 0xE3, 0xD2, 0x41, 0xB6, 0x1C, 0x20},
            std::vector<uint8_t>{0x01, 0x03, 0x08, 0x3F, 0x75, 0xE3, 0xD2, 0x41, 0xB6, 0x1C, 0x20, 0xA0, 0xC7})
    )
);

TEST(ParseFloat, basic) {
    srp::core::RS485 rs485;
    std::vector<uint8_t> data = { 0x3F, 0x75, 0xF0, 0x7B};
    EXPECT_NEAR(rs485.parseFloat(data), 0.960701, 0.000001);
}
TEST(ParseFloat, basic2) {
    srp::core::RS485 rs485;
    std::vector<uint8_t> data = { 0x41, 0xB5, 0xC0, 0x79};
    EXPECT_NEAR(rs485.parseFloat(data), 22.719, 0.0001);
}