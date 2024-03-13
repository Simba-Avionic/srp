/**
 * @file test_dtc_msg_factory.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "mw/temp/subscribe_msg/subscribe_header.h"
#include "mw/temp/subscribe_msg/subscribe_msg_factory.h"

TEST(SUBSCRIBE_MSG_FACTORIES, SUBSCRIBE_MSG_FACTORIES_TEST) {
    simba::mw::temp::SubMsgFactory factory;
    std::vector<uint8_t> payload = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5};
    uint16_t id = 0x0001;
    simba::mw::temp::SubscribeHeader hdr{id};
    std::vector<uint8_t> data = factory.GetBuffer(
        std::make_shared<simba::mw::temp::SubscribeHeader>(hdr), payload);
    auto hdr2 = factory.GetHeader(data);
    EXPECT_EQ(hdr2->GetServiceID(), hdr.GetServiceID());
    EXPECT_EQ(hdr2->GetLength(), hdr.GetLength());
}