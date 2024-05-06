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
#include "mw/i2c_service/data/i2c_factory.h"

TEST(HEADER, CONSTRUCTOR_CHECK) {
    const uint16_t service_id = 0x11;
    const auto action =  simba::i2c::ACTION::Write;
    const uint16_t address = 0x22;
simba::i2c::Header hdr{action, address, service_id};
EXPECT_EQ(hdr.GetAction(), action);
EXPECT_EQ(hdr.GetServiceId(), service_id);
EXPECT_EQ(hdr.GetAddress(), address);
}

TEST(FACTORY, FACTORY_CHECK_1) {
    const uint16_t service_id = 0x22;
    const auto action =  simba::i2c::ACTION::Write;
    const uint16_t address = 0x11;
    std::vector<uint8_t> payload = {0, 1, 2, 3, 4};
    auto buf = simba::i2c::I2CFactory::GetBuffer(
                 std::make_shared<simba::i2c::Header>(action, address, service_id), payload);
    auto hdr2 = simba::i2c::I2CFactory::GetHeader(buf);
    auto payload2 = simba::i2c::I2CFactory::GetPayload(buf);
    EXPECT_EQ(action, hdr2->GetAction());
    EXPECT_EQ(service_id, hdr2->GetServiceId());
    EXPECT_EQ(address, hdr2->GetAddress());
    EXPECT_EQ(payload.size(), hdr2->GetPayloadSize());
    EXPECT_EQ(payload2.size(), payload.size());
    EXPECT_EQ(payload, payload2);
}
