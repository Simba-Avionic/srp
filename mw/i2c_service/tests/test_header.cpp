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
                        std::tuple<srp::i2c::ACTION, uint16_t>> {
};

// Definicja parametrów testu
INSTANTIATE_TEST_SUITE_P(HeaderConstructorTestParameters, HeaderConstructorTest,
    ::testing::Values(
        std::make_tuple(srp::i2c::ACTION::kWrite, 0x00),
        std::make_tuple(srp::i2c::ACTION::kRES, 0xFF),
        std::make_tuple(srp::i2c::ACTION::kRead, 0xFA),
        std::make_tuple(srp::i2c::ACTION::kPageWrite, 0x55)
    )
);

TEST_P(HeaderConstructorTest, ConstructorCheck) {
    auto params = GetParam();
    const srp::i2c::ACTION action = std::get<0>(params);
    const uint8_t address = std::get<1>(params);
    srp::i2c::Header hdr{action, address};
    EXPECT_EQ(hdr.GetAction(), action);
    EXPECT_EQ(hdr.GetAddress(), address);
    EXPECT_EQ(hdr.GetPayloadSize(), 0);
}

TEST(FACTORY, FactoryCheckTestWithoudPayload) {
    const auto action =  srp::i2c::ACTION::kWrite;
    const uint16_t address = 0x11;
    std::vector<uint8_t> payload = {};
    auto buf = srp::i2c::I2CFactory::GetBuffer(
                 std::make_shared<srp::i2c::Header>(action, address), payload);
    auto hdr2 = srp::i2c::I2CFactory::GetHeader(buf);
    auto payload2 = srp::i2c::I2CFactory::GetPayload(buf);
    EXPECT_EQ(action, hdr2->GetAction());
    EXPECT_EQ(address, hdr2->GetAddress());
    EXPECT_EQ(payload.size(), hdr2->GetPayloadSize());
    EXPECT_EQ(payload2.has_value(), false);
}

class FactoryCheckTest : public ::testing::TestWithParam<
            std::tuple<srp::i2c::ACTION, uint16_t, std::vector<uint8_t>>> {
};

INSTANTIATE_TEST_SUITE_P(FactoryCheckTestParameters, FactoryCheckTest,
    ::testing::Values(
        std::make_tuple(srp::i2c::ACTION::kWrite, 0xFF, std::vector<uint8_t>{0}),
        std::make_tuple(srp::i2c::ACTION::kPageWrite, 0xAF, std::vector<uint8_t>{0, 1}),
        std::make_tuple(srp::i2c::ACTION::kRead, 0xFB, std::vector<uint8_t>{0, 1, 2}),
        std::make_tuple(srp::i2c::ACTION::kRES, 0x14, std::vector<uint8_t>{0, 1, 2, 3, 4})
    )
);

TEST_P(FactoryCheckTest, FactoryCheck) {
    const auto [action, address, payload] = GetParam();
    auto buf = srp::i2c::I2CFactory::GetBuffer(
                 std::make_shared<srp::i2c::Header>(action, address), payload);
    auto hdr2 = srp::i2c::I2CFactory::GetHeader(buf);
    auto payload2 = srp::i2c::I2CFactory::GetPayload(buf);
    EXPECT_EQ(action, hdr2->GetAction());
    EXPECT_EQ(address, hdr2->GetAddress());
    EXPECT_EQ(payload.size(), hdr2->GetPayloadSize());
    EXPECT_EQ(payload.size(), payload2.value().size());
    EXPECT_EQ(payload, payload2);
}

TEST(HEADER, SETTER_PAYLOAD_SIZE) {
    srp::i2c::Header hdr(srp::i2c::ACTION::kPageWrite, 0);
    hdr.SetPaylaodSize(12);
    EXPECT_EQ(hdr.GetPayloadSize(), 12);
}

TEST(HEADER, SETTER_PAYLOAD_SIZE2) {
    auto buf = srp::i2c::I2CFactory::GetBuffer(
                std::make_shared<srp::i2c::Header>(srp::i2c::ACTION::kPageWrite, 0), {123});
    auto hdr = srp::i2c::I2CFactory::GetHeader(buf);
    EXPECT_EQ(hdr->GetPayloadSize(), 1);
}
