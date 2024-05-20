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
#include <iostream>

#include "mw/gpio_server/data/header.hpp"
struct gpio_params{
    uint16_t service_id;
    uint16_t pin_id;
    uint8_t value;
};

TEST(GPIO_DATA_STRUCTURE, CONSTRUCTOR_CHECK) {
    
std::vector<gpio_params> payload = {
    {0x1111, 0x11, 0x1},
    {12, 1, 0},
    {7, 5, 0}
};
for(const auto& p : payload) {
    simba::gpio::Header hdr{p.service_id, p.pin_id, p.value};
    EXPECT_EQ(hdr.GetServiceID(), p.service_id);
    EXPECT_EQ(hdr.GetPinID(), p.pin_id);
    EXPECT_EQ(hdr.GetValue(), p.value);
}
}


TEST(GPIO_MSG_FACTORY, CHECK_BUFFOR) {
    simba::gpio::Header hdr(12, 12, 1);
    auto data = hdr.GetBuffor();
    simba::gpio::Header hdr2(0x00, 0x00, 0x0);
    hdr2.SetBuffor(data);
    EXPECT_EQ(hdr.GetServiceID(), hdr2.GetServiceID());
    EXPECT_EQ(hdr.GetPinID(), hdr2.GetPinID());
    EXPECT_EQ(hdr.GetValue(), hdr2.GetValue());
}
