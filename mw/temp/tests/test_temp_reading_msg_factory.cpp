/**
 * @file test_temp_reading_msg_factory.cpp
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

#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"

class TEMP_READING_MSG_FACTORY : public ::testing::TestWithParam<::std::vector<srp::mw::temp::TempReading>> {
 protected:
};

INSTANTIATE_TEST_SUITE_P(TEMP_READING_MSG_FACTORY_PARAMS,
                         TEMP_READING_MSG_FACTORY,
                         ::testing::Values(
                            std::vector<srp::mw::temp::TempReading>{{0, 0.0}, {1, -1.1}, {2, 123.654}},
                            std::vector<srp::mw::temp::TempReading>{{0, 0.0}},
                            std::vector<srp::mw::temp::TempReading>{{1, -1.1}, {2, 123.654}},
                            std::vector<srp::mw::temp::TempReading>{{0, 0.0}, {1, -1.1}, {2, 123.654}, {2, 123.654}},
                            std::vector<srp::mw::temp::TempReading>{{255, 10000000}},
                            std::vector<srp::mw::temp::TempReading>{{255, -100000}}
));

TEST_P(TEMP_READING_MSG_FACTORY, TEMP_READING_MSG_FACTORY_TEST) {
    std::vector<srp::mw::temp::TempReading> payload = GetParam();

    std::vector<uint8_t> raw_data = srp::mw::temp::TempReadingMsgFactory::GetBuffer(payload);
    const auto payload2 = srp::mw::temp::TempReadingMsgFactory::GetPayload(raw_data);
    EXPECT_EQ(payload, payload2);
}
