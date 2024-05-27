/**
 * @file test_temp_reading_msg_factory.cpp
 * @author Jacek Kukawski (jacekka6@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"

class TEMP_READING_MSG_FACTORY : public ::testing::TestWithParam<::std::vector<simba::mw::temp::TempReading>> {
    protected:
        simba::mw::temp::TempReadingMsgFactory factory;
};

INSTANTIATE_TEST_SUITE_P(TEMP_READING_MSG_FACTORY_PARAMS,
                         TEMP_READING_MSG_FACTORY,
                         ::testing::Values(
                             std::vector<simba::mw::temp::TempReading>{{0, 0.0}, {1, -1.1}, {2, 123.654}}));

TEST_P(TEMP_READING_MSG_FACTORY, TEMP_READING_MSG_FACTORY_TEST) {
    std::vector<simba::mw::temp::TempReading> payload = GetParam();

    std::vector<uint8_t> raw_data = factory.GetBuffer(payload);
    const auto payload2 = factory.GetPayload(raw_data);

    EXPECT_EQ(payload[0], payload2[0]);
    EXPECT_EQ(payload[1], payload2[1]);
    EXPECT_EQ(payload[2], payload2[2]);
}