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

// Definicja testu parametryzowanego
class HeaderConstructorTest : public ::testing::TestWithParam<
                        std::tuple<uint16_t, simba::i2c::ACTION, uint16_t>> {
};

// Definicja parametrów testu
INSTANTIATE_TEST_SUITE_P(HeaderConstructorTestParameters, HeaderConstructorTest,
    ::testing::Values(
        std::make_tuple(0xAF, simba::i2c::ACTION::Write, 0x00),
        std::make_tuple(0x11, simba::i2c::ACTION::RES, 0xFF),
        std::make_tuple(0x55, simba::i2c::ACTION::Read, 0xFA),
        std::make_tuple(0xFF, simba::i2c::ACTION::PageRead, 0xAF),
        std::make_tuple(0xFA, simba::i2c::ACTION::PageWrite, 0x55)
    )
);

TEST_P(HeaderConstructorTest, ConstructorCheck) {
    auto params = GetParam();
    const uint16_t service_id = std::get<0>(params);
    const simba::i2c::ACTION action = std::get<1>(params);
    const uint8_t address = std::get<2>(params);
    simba::i2c::Header hdr{action, address, service_id};
    EXPECT_EQ(hdr.GetAction(), action);
    EXPECT_EQ(hdr.GetServiceId(), service_id);
    EXPECT_EQ(hdr.GetAddress(), address);
}

TEST(FACTORY, FactoryCheckTestWithoudPayload) {
    const uint16_t service_id = 0x22;
    const auto action =  simba::i2c::ACTION::Write;
    const uint16_t address = 0x11;
    std::vector<uint8_t> payload = {};
    auto buf = simba::i2c::I2CFactory::GetBuffer(
                 std::make_shared<simba::i2c::Header>(action, address, service_id), payload);
    auto hdr2 = simba::i2c::I2CFactory::GetHeader(buf);
    auto payload2 = simba::i2c::I2CFactory::GetPayload(buf);
    EXPECT_EQ(action, hdr2->GetAction());
    EXPECT_EQ(service_id, hdr2->GetServiceId());
    EXPECT_EQ(address, hdr2->GetAddress());
    EXPECT_EQ(payload.size(), hdr2->GetPayloadSize());
    EXPECT_EQ(payload2.has_value(), false);
}

class FactoryCheckTest : public ::testing::TestWithParam<
            std::tuple<uint16_t, simba::i2c::ACTION, uint16_t, std::vector<uint8_t>>> {
};

INSTANTIATE_TEST_SUITE_P(FactoryCheckTestParameters, FactoryCheckTest,
    ::testing::Values(
        std::make_tuple(0xFF, simba::i2c::ACTION::PageRead, 0xFF, std::vector<uint8_t>{0}),
        std::make_tuple(0xAF, simba::i2c::ACTION::PageWrite, 0xAF, std::vector<uint8_t>{0, 1}),
        std::make_tuple(0xFA, simba::i2c::ACTION::Read, 0xFB, std::vector<uint8_t>{0, 1, 2}),
        std::make_tuple(0x53, simba::i2c::ACTION::RES, 0x14, std::vector<uint8_t>{0, 1, 2, 3, 4})
    )
);

TEST_P(FactoryCheckTest, FactoryCheck) {
    const auto [service_id, action, address, payload] = GetParam();
    auto buf = simba::i2c::I2CFactory::GetBuffer(
                 std::make_shared<simba::i2c::Header>(action, address, service_id), payload);
    auto hdr2 = simba::i2c::I2CFactory::GetHeader(buf);
    auto payload2 = simba::i2c::I2CFactory::GetPayload(buf);
    EXPECT_EQ(action, hdr2->GetAction());
    EXPECT_EQ(service_id, hdr2->GetServiceId());
    EXPECT_EQ(address, hdr2->GetAddress());
    EXPECT_EQ(payload.size(), hdr2->GetPayloadSize());
    EXPECT_EQ(payload2.value().size(), payload.size());
    EXPECT_EQ(payload, payload2);
}

