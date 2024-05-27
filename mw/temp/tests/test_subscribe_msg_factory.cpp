/**
 * @file test_subscribe_msg_factory.cpp
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "mw/temp/subscribe_msg/subscribe_header.h"
#include "mw/temp/subscribe_msg/subscribe_msg_factory.h"

class SubscribeMsgFactoriesTest : public ::testing::TestWithParam<
                        std::tuple<std::vector<uint8_t>, uint16_t>> {
};

INSTANTIATE_TEST_SUITE_P(SubscribeMsgFactoriesTestParameters, SubscribeMsgFactoriesTest,
    ::testing::Values(
        std::make_tuple(std::vector<uint8_t>{0x0, 0x1, 0x2, 0x3, 0x4, 0x5}, 0x0001),
        std::make_tuple(std::vector<uint8_t>{0x0, 0x0, 0x0, 0x0, 0x0, 0x5}, 0x0000),
        std::make_tuple(std::vector<uint8_t>{255, 255, 255, 255, 255, 255}, 32767)
    )
);

TEST_P(SubscribeMsgFactoriesTest, CONSTRUCTOR_CHECK) {
    uint8_t length = 0x2;
    auto params = GetParam();
    const uint16_t id = std::get<1>(params);
    simba::mw::temp::SubscribeHeader hdr{id};
    EXPECT_EQ(id, hdr.GetServiceID());
    EXPECT_EQ(length, hdr.GetLength());
}

TEST_P(SubscribeMsgFactoriesTest, SUBSCRIBE_MSG_FACTORIES_TEST) {
    simba::mw::temp::SubMsgFactory factory;
    auto params = GetParam();
    std::vector<uint8_t> payload = std::get<0>(params);
    const uint16_t id = std::get<1>(params);
    simba::mw::temp::SubscribeHeader hdr{id};
    const std::vector<uint8_t> data = factory.GetBuffer(
        std::make_shared<simba::mw::temp::SubscribeHeader>(hdr), std::move(payload));
    const auto hdr2 = factory.GetHeader(data);
    EXPECT_EQ(hdr2->GetServiceID(), hdr.GetServiceID());
    EXPECT_EQ(hdr2->GetLength(), hdr.GetLength());
}
