/**
 * @file test_data_hdr.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <gtest/gtest.h>

#include "diag/dtc/data/dtc_header.hpp"

TEST(DTC_DATA_STRUCTURE, CONSTRUCTOR_CHECK) {
    uint16_t id = 0x1111;
    uint8_t status = 0x7;
simba::diag::dtc::DtcHeader hdr{id, status};
EXPECT_EQ(id, hdr.GetDtcID());
EXPECT_EQ(status, hdr.GetDtcStatus());
EXPECT_EQ(04, hdr.GetLength());
}
