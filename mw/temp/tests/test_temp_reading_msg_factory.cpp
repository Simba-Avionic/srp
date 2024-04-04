/**
 * @file test_temp_reading_msg_factory.cpp
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"

TEST(TEMP_READING_MSG_FACTORY, TEMP_READING_MSG_FACTORY_TEST) {
    simba::mw::temp::TempReadingMsgFactory factory;
    std::vector<simba::mw::temp::TempReading> payload = {
        {0, 0.0},
        {1, -1.1},
        {2, 123.654}
    };

    std::vector<uint8_t> raw_data = factory.GetBuffer(payload);
    auto payload2 = factory.GetPayload(raw_data);

    EXPECT_EQ(payload[0], payload2[0]);
    EXPECT_EQ(payload[1], payload2[1]);
    EXPECT_EQ(payload[2], payload2[2]);
}
