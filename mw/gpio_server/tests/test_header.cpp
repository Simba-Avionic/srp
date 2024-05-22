/**
 * @file test_header.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <gtest/gtest.h>

#include "mw/gpio_server/data/header.hpp"

class HeaderConstructorTest : public ::testing::TestWithParam<
                        std::tuple<uint16_t, uint16_t,  uint8_t>> {
};

INSTANTIATE_TEST_SUITE_P(HeaderConstructorTestParameters, HeaderConstructorTest,
    ::testing::Values(
        std::make_tuple(0x1111, 0x11, 0x1),
        std::make_tuple(12, 1, 0),
        std::make_tuple(7, 5, 0)
    )
);

TEST_P(HeaderConstructorTest, CONSTRUCTOR_CHECK) {
    auto params = GetParam();
    const uint16_t service_id = std::get<0>(params);
    const uint16_t pin_id = std::get<1>(params);
    const uint8_t value = std::get<2>(params);
    simba::gpio::Header hdr{service_id, pin_id, value};
    EXPECT_EQ(hdr.GetServiceID(), service_id);
    EXPECT_EQ(hdr.GetPinID(), pin_id);
    EXPECT_EQ(hdr.GetValue(), value);
}

TEST_P(HeaderConstructorTest, CHECK_BUFFOR) {
    auto params = GetParam();
    const uint16_t service_id = std::get<0>(params);
    const uint16_t pin_id = std::get<1>(params);
    const uint8_t value = std::get<2>(params);
    simba::gpio::Header hdr{service_id, pin_id, value};
    auto data = hdr.GetBuffor();
    simba::gpio::Header hdr2(0x00, 0x00, 0x0);
    hdr2.SetBuffor(data);
    EXPECT_EQ(hdr.GetServiceID(), hdr2.GetServiceID());
    EXPECT_EQ(hdr.GetPinID(), hdr2.GetPinID());
    EXPECT_EQ(hdr.GetValue(), hdr2.GetValue());
}
