/**
 * @file test_header.cpp
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <gtest/gtest.h>

#include "mw/gpio_server/data/header.hpp"

class HeaderConstructorTest : public ::testing::TestWithParam<
                        std::tuple<uint16_t,  uint8_t, simba::gpio::ACTION>> {
};

INSTANTIATE_TEST_SUITE_P(HeaderConstructorTestParameters, HeaderConstructorTest,
    ::testing::Values(
        std::make_tuple(0x11, 0x1, simba::gpio::ACTION::GET),
        std::make_tuple(1, 0, simba::gpio::ACTION::SET),
        std::make_tuple(5, 0, simba::gpio::ACTION::RES)
    )
);

TEST_P(HeaderConstructorTest, CONSTRUCTOR_CHECK) {
    auto params = GetParam();
    const uint8_t actuatorID = std::get<0>(params);
    const uint8_t value = std::get<1>(params);
    const simba::gpio::ACTION action = std::get<2>(params);
    simba::gpio::Header hdr{actuatorID, value, action};
    EXPECT_EQ(hdr.GetAction(), action);
    EXPECT_EQ(hdr.GetActuatorID(), actuatorID);
    EXPECT_EQ(hdr.GetValue(), value);
}

TEST_P(HeaderConstructorTest, CHECK_BUFFOR) {
    auto params = GetParam();
    const uint8_t actuatorID = std::get<0>(params);
    const uint8_t value = std::get<1>(params);
    const simba::gpio::ACTION action = std::get<2>(params);
    simba::gpio::Header hdr{actuatorID, value, action};
    auto data = hdr.GetBuffor();
    simba::gpio::Header hdr2(0x00, 0x0, simba::gpio::ACTION::GET);
    hdr2.SetBuffor(data);
    EXPECT_EQ(hdr.GetActuatorID(), hdr2.GetActuatorID());
    EXPECT_EQ(hdr.GetValue(), hdr2.GetValue());
}
