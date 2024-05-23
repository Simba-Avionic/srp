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

TEST(GPIO_DATA_STRUCTURE, CONSTRUCTOR_CHECK) {
    uint16_t pin_id = 0x11;
    int8_t value = 0x1;
simba::gpio::Header hdr{pin_id, 0, simba::gpio::ACTION::GET};
hdr.SetValue(value);
EXPECT_EQ(hdr.GetActuatorID(), pin_id);
EXPECT_EQ(hdr.GetValue(), value);
    pin_id = 1;
    value = 0;
simba::gpio::Header hdr2{pin_id, value, simba::gpio::ACTION::GET};
EXPECT_EQ(hdr2.GetActuatorID(), pin_id);
EXPECT_EQ(hdr2.GetValue(), value);
    pin_id = 5;
    value = 0;
simba::gpio::Header hdr3{pin_id, value, simba::gpio::ACTION::GET};
EXPECT_EQ(hdr3.GetActuatorID(), pin_id);
EXPECT_EQ(hdr3.GetValue(), value);
}

TEST(GPIO_MSG_FACTORY, CHECK_BUFFOR) {
    simba::gpio::Header hdr(12, 1, simba::gpio::ACTION::GET);
    auto data = hdr.GetBuffor();
    simba::gpio::Header hdr2(0x00, 0x0, simba::gpio::ACTION::GET);
    hdr2.SetBuffor(data);
    EXPECT_EQ(hdr.GetActuatorID(), hdr2.GetActuatorID());
    EXPECT_EQ(hdr.GetValue(), hdr2.GetValue());
}
