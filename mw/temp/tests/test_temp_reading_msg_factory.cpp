/**
 * @file test_temp_reading_msg_factory.cpp
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

class TempReadingMsgFactoryTest : public ::testing::TestWithParam<
                        std::vector<simba::mw::temp::TempReading>> {
};

INSTANTIATE_TEST_SUITE_P(TempReadingMsgFactoryTestParameters, TempReadingMsgFactoryTest,
    ::testing::Values(
        std::vector<simba::mw::temp::TempReading>{{0, 0.0}},
        std::vector<simba::mw::temp::TempReading>{{1, -1.1}},
        std::vector<simba::mw::temp::TempReading>{{2, 123.654}},
        std::vector<simba::mw::temp::TempReading>{{255, 100000000000000000000000000000000000000}},
        std::vector<simba::mw::temp::TempReading>{{255, -100000000000000000000000000000000000000}}        
    )
);

TEST_P(TempReadingMsgFactoryTest, TEMP_READING_MSG_FACTORY_TEST) {
    simba::mw::temp::TempReadingMsgFactory factory;
    auto params = GetParam();
    std::vector<simba::mw::temp::TempReading> payload = params;

    std::vector<uint8_t> raw_data = factory.GetBuffer(payload);
    const auto payload2 = factory.GetPayload(raw_data);

    EXPECT_EQ(payload, payload2);
}
