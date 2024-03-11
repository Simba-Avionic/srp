/**
 * @file test_data_hdr.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <gtest/gtest.h>

#include "mw/tempService/subscribe_msg/subscribe_header.h"

TEST(SUBSCRIBE_HEADER, CONSTRUCTOR_CHECK) {
    uint16_t id = 0x0010;
    simba::mw::temp::SubscribeHeader hdr{id};
    const std::vector<uint8_t> payload{0x01, 0x02, 0x03};
    EXPECT_EQ(id, hdr.GetServiceID());
    EXPECT_EQ(02, hdr.GetLength());
}
