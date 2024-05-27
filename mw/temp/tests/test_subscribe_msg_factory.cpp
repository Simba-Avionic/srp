/**
 * @file test_subscribe_msg_factory.cpp
 * @author Jacek Kukawski (jacekka6@gmail.com)
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

class SUBSCRIBE_MSG_FACTORIES : public ::testing::TestWithParam<
                        std::tuple<uint16_t, std::vector<uint8_t>>> {
    protected:
        simba::mw::temp::SubMsgFactory factory;
        uint16_t id;
        std::vector<uint8_t> payload;

        void SetUp() override {
            auto params = GetParam();
            id = std::get<0>(params);
            payload = std::get<1>(params);
        }
};

INSTANTIATE_TEST_SUITE_P(SUBSCRIBE_MSG_FACTORIES_TEST,
                         SUBSCRIBE_MSG_FACTORIES,
                         ::testing::Values(
                            std::make_tuple(0x0001, std::vector<uint8_t>{0x0, 0x1, 0x2, 0x3, 0x4, 0x5}),
                            std::make_tuple(std::vector<uint8_t>{0x0, 0x1, 0x2, 0x3, 0x4, 0x5}, 0x0001),
                            std::make_tuple(std::vector<uint8_t>{0x0, 0x0, 0x0, 0x0, 0x0, 0x5}, 0x0000),
                            std::make_tuple(std::vector<uint8_t>{255, 255, 255, 255, 255, 255}, 32767)
                         )
);

TEST_P(SUBSCRIBE_MSG_FACTORIES, SUBSCRIBE_MSG_FACTORIES_TEST) {
    simba::mw::temp::SubscribeHeader hdr{id};
    const std::vector<uint8_t> data = factory.GetBuffer(
        std::make_shared<simba::mw::temp::SubscribeHeader>(hdr), std::move(payload));
    const auto hdr2 = factory.GetHeader(data);
    EXPECT_EQ(hdr2->GetServiceID(), hdr.GetServiceID());
    EXPECT_EQ(hdr2->GetLength(), hdr.GetLength());
}
