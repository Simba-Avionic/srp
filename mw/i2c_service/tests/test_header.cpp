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

#include "mw/i2c_service/data/header.h"

TEST(HEADER, CONSTRUCTOR_CHECK) {
    const uint16_t service_id = 0x11;
    const auto action =  simba::i2c::ACTION::Write;
    const uint16_t address = 0x22;
simba::i2c::Header hdr{action, address, service_id};
EXPECT_EQ(hdr.GetAction(), action);
EXPECT_EQ(hdr.GetServiceId(), service_id);
EXPECT_EQ(hdr.GetAddress(), address);
}


TEST(FACTORY, FACTORY_CHECK) {
    const uint16_t service_id = 0x22;
    const auto action =  simba::i2c::ACTION::Write;
    const uint16_t address = 0x11;
    simba::i2c::I2CFactory factory;
    simba::i2c::Header hdr{action, address, service_id};
    std::vector<uint8_t> payload = {0, 1, 2, 3, 4};
    auto buf = factory.GetBuffer(std::make_shared<simba::i2c::Header>(hdr), payload);
    auto hdr2 = factory.GetHeader(buf);
    auto payload2 = factory.GetPayload(buf);
    EXPECT_EQ(hdr.GetAction(), hdr2->GetAction());
    EXPECT_EQ(hdr.GetServiceId(), hdr2->GetServiceId());
    EXPECT_EQ(hdr.GetAddress(), hdr2->GetAddress());
    EXPECT_EQ(payload, payload2);
}
