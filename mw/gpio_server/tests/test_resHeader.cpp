/**
 * @file test_resHeader.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "mw/gpio_server/data/resHeader.hpp"

TEST(GPIO_ResDATA_STRUCTURE, CONSTRUCTOR_CHECK) {
    uint16_t pin_id = 0x11;
    uint8_t value = 0x1;
simba::gpio::ResHeader hdr{pin_id, value};
EXPECT_EQ(hdr.GetPinID(), pin_id);
EXPECT_EQ(hdr.GetValue(), value);
}


TEST(GPIO_RES_MSG_FACTORY, CHECK_BUFFOR) {
    simba::gpio::ResHeader hdr(12, 1);
    auto data = hdr.GetBuffor();
    simba::gpio::ResHeader hdr2(0x00, 0x0);
    hdr2.SetBuffor(data);
    EXPECT_EQ(hdr.GetPinID(), hdr2.GetPinID());
    EXPECT_EQ(hdr.GetValue(), hdr2.GetValue());
}