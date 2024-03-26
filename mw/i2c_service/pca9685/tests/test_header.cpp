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

#include "mw/i2c_service/pca9685/data/servo_hdr.hpp"

TEST(GPIO_DATA_STRUCTURE, CONSTRUCTOR_CHECK) {
    uint8_t actuator_id = 0x11;
    uint16_t pos = 0x1;
simba::i2c::ServoHdr hdr{actuator_id, pos};
EXPECT_EQ(hdr.GetActuatorID(), actuator_id);
EXPECT_EQ(hdr.GetPosition(), pos);
EXPECT_EQ(hdr.GetMode(), simba::i2c::smode_t::AUTO);
    actuator_id = 0x22;
    uint16_t pos2 = 3500;
simba::i2c::ServoHdr hdr2{actuator_id, pos2, simba::i2c::smode_t::MAN};
EXPECT_EQ(hdr2.GetActuatorID(), actuator_id);
EXPECT_EQ(hdr2.GetPosition(), pos2);
EXPECT_EQ(hdr2.GetMode(), simba::i2c::smode_t::MAN);
}
