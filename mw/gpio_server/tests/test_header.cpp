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
                        std::tuple<uint16_t,  uint8_t, srp::gpio::ACTION>> {
};

INSTANTIATE_TEST_SUITE_P(HeaderConstructorTestParameters, HeaderConstructorTest,
    ::testing::Values(
        std::make_tuple(0x11, 0x1, srp::gpio::ACTION::GET),
        std::make_tuple(1, 0, srp::gpio::ACTION::SET),
        std::make_tuple(5, 0, srp::gpio::ACTION::RES)
    )
);

TEST_P(HeaderConstructorTest, CONSTRUCTOR_CHECK) {
    auto params = GetParam();
    const uint8_t actuatorID = std::get<0>(params);
    const uint8_t value = std::get<1>(params);
    const srp::gpio::ACTION action = std::get<2>(params);
    srp::gpio::Header hdr{actuatorID, value, action};
    EXPECT_EQ(hdr.GetAction(), action);
    EXPECT_EQ(hdr.GetActuatorID(), actuatorID);
    EXPECT_EQ(hdr.GetValue(), value);
}

TEST_P(HeaderConstructorTest, CHECK_BUFFOR) {
    auto params = GetParam();
    const uint8_t actuatorID = std::get<0>(params);
    const uint8_t value = std::get<1>(params);
    const srp::gpio::ACTION action = std::get<2>(params);
    srp::gpio::Header hdr{actuatorID, value, action};
    auto data = hdr.GetBuffor();
    srp::gpio::Header hdr2(0x00, 0x0, srp::gpio::ACTION::GET);
    hdr2.SetBuffor(data);
    EXPECT_EQ(hdr.GetActuatorID(), hdr2.GetActuatorID());
    EXPECT_EQ(hdr.GetValue(), hdr2.GetValue());
}
TEST_P(HeaderConstructorTest, CHECK_SET) {
    auto params = GetParam();
    const uint8_t actuatorID = std::get<0>(params);
    const uint8_t value = std::get<1>(params);
    const srp::gpio::ACTION action = std::get<2>(params);
    srp::gpio::Header hdr{0, 0, srp::gpio::ACTION::GET};
    hdr.SetPinID(actuatorID);
    hdr.SetAction(action);
    hdr.SetValue(value);
    EXPECT_EQ(hdr.GetActuatorID(), actuatorID);
    EXPECT_EQ(hdr.GetValue(), value);
    EXPECT_EQ(hdr.GetActuatorID(), actuatorID);
}
