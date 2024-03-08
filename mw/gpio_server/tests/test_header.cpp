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

TEST(GPIO_DATA_STRUCTURE, CONSTRUCTOR_CHECK) {
    uint16_t service_id = 0x1111;
    uint16_t pin_id = 0x11;
    uint8_t value = 0x1;
simba::gpio::Header hdr{service_id, pin_id, value};
EXPECT_EQ(hdr.GetServiceID(), service_id);
EXPECT_EQ(hdr.GetPinID(), pin_id);
EXPECT_EQ(hdr.GetValue(), value);
}


TEST(GPIO_MSG_FACTORY,CHECK_BUFFOR) {
    simba::gpio::Header hdr(12,12,1);
    auto data=hdr.GetBuffor();
    simba::gpio::Header hdr2(0x00,0x00,0x0);
    hdr2.SetBuffor(data);
    EXPECT_EQ(hdr.GetServiceID(),hdr2.GetServiceID());
    EXPECT_EQ(hdr.GetPinID(),hdr2.GetPinID());
    EXPECT_EQ(hdr.GetValue(),hdr2.GetValue());
}