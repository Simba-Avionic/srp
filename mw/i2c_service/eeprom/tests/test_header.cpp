/**
 * @file test_header.cpp
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

#include "mw/i2c_service/eeprom/data/eeprom_hdr.hpp"

TEST(GPIO_DATA_STRUCTURE, CONSTRUCTOR_CHECK) {
    uint16_t client_id = 0x11;
    uint8_t transmition_id = 0x22;
    simba::i2c::operation_t operation = simba::i2c::operation_t::WRITE;
    uint8_t address = 0x00;
    simba::i2c::EepromHdr hdr{client_id, transmition_id, operation, address};

    EXPECT_EQ(hdr.GetClientID(), client_id);
    EXPECT_EQ(hdr.GetOperationType(), operation);
    EXPECT_EQ(hdr.GetTransmitionID(), transmition_id);
    EXPECT_EQ(hdr.GetAddress(), address);
}
