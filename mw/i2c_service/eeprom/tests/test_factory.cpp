/**
 * @file test_factory.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <gtest/gtest.h>
#include <iostream>

#include "mw/i2c_service/eeprom/factory/eeprom_factory.hpp"

TEST(GPIO_DATA_STRUCTURE, CONSTRUCTOR_CHECK) {
    uint16_t client_id = 0x11;
    uint8_t transmition_id = 0x22;
    uint8_t address = 0x10;
    simba::i2c::operation_t operation = simba::i2c::operation_t::WRITE;
    simba::i2c::EepromHdr hdr{client_id, transmition_id, operation, address};
    std::vector<uint8_t> payload = {0, 1, 2, 3, 4};
    simba::i2c::EepromMsgFactory factory_;
    auto buf = factory_.GetBuffer(std::make_shared<simba::i2c::EepromHdr>(hdr), payload);
    auto hdr2 = factory_.GetHeader(buf);
    auto pay = factory_.GetPayload(buf);
    EXPECT_EQ(hdr.GetClientID(), hdr2->GetClientID());
    EXPECT_EQ(hdr.GetOperationType(), hdr2->GetOperationType());
    EXPECT_EQ(hdr.GetTransmitionID(), hdr2->GetTransmitionID());
    EXPECT_EQ(payload, pay);
    EXPECT_EQ(hdr.GetAddress(),hdr2->GetAddress());
}
