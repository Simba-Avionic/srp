/**
 * @file test_subscribe_msg.cpp
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "mw/temp/subscribe_msg/subscribe_header.h"

TEST(SUBSCRIBE_HEADER, CONSTRUCTOR_CHECK) {
    const uint16_t id = 0x0010;
    simba::mw::temp::SubscribeHeader hdr{id};
    const std::vector<uint8_t> payload{0x01, 0x02, 0x03};
    EXPECT_EQ(id, hdr.GetServiceID());
    EXPECT_EQ(02, hdr.GetLength());
}
