/**
 * @file test_dlt.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Unit tests for logger data
 * @version 0.1
 * @date 2024-06-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <cstring>
#include "gtest/gtest.h"

#include "mw/logger/code/data/dlt_frame.h"
#include "mw/logger/code/data/dlt_log_type.h"
#include "mw/logger/code/data/dlt_string.h"
#include "mw/logger/code/data/idlt_arg.h"
#include "mw/logger/code/data/idlt_frame.h"

class DltFrameStringTest : public ::testing::TestWithParam<
                        std::tuple<uint32_t, std::string, std::string, simba::dlt::data::DLTLogType, std::string>> {
};

INSTANTIATE_TEST_SUITE_P(DltFrameStringTestParameters, DltFrameStringTest,
    ::testing::Values(
        std::make_tuple(0, "0", "HOME", simba::dlt::data::DLTLogType::kDLTDebug, "test_msg"),
        std::make_tuple(4294967295, "1", "APP-", simba::dlt::data::DLTLogType::kDLTError, "error"),
        std::make_tuple(0, "21", "JAKU", simba::dlt::data::DLTLogType::kDLTInfo, "info"),
        std::make_tuple(0, "312", "TEMP", simba::dlt::data::DLTLogType::kDLTWarning, "warning")
    )
);

TEST_P(DltFrameStringTest, DLT_FRAME_WITH_DLT_STRING) {
    auto params = GetParam();
    uint32_t timestamp = std::get<0>(params);
    std::string ecu_id = std::get<1>(params);
    std::string app_id = std::get<2>(params);
    const simba::dlt::data::DLTLogType type = std::get<3>(params);
    std::string log = std::get<4>(params);
    simba::dlt::data::DltString payload_{log};
    simba::dlt::data::DltFrame frame{timestamp, ecu_id, app_id, type, payload_};
    auto parsed_opt = frame.ParseFrame();
    auto parsed = parsed_opt.value();
    EXPECT_EQ(parsed.at(0), 0x37);
    EXPECT_EQ(parsed.at(1), 0x01);
    int len = (parsed.at(2) << 8U) | parsed.at(3);
    EXPECT_EQ(len, payload_.Length() + 0x16);
    int i = 4;
    std::string ecu_parsed{parsed.begin() + i, parsed.begin() + i + ecu_id.length()};
    EXPECT_EQ(ecu_parsed, ecu_id);
    i += ecu_id.length();
    uint32_t timestamp_parsed{0};
    std::memcpy(&timestamp_parsed, parsed.data() + i, sizeof(uint32_t));
    i += 4;
    EXPECT_EQ(timestamp_parsed, timestamp);
    EXPECT_EQ(parsed.at(i++), type);
    EXPECT_EQ(parsed.at(i++), 0x01);
    std::string app_parsed{parsed.begin() + i, parsed.begin() + i + app_id.length()};
    EXPECT_EQ(app_parsed, app_id);
    i += app_id.length();
    std::string none_parsed{parsed.begin() + i, parsed.begin() + i + 4};
    EXPECT_EQ(none_parsed, "NONE");
    i += 4;
    std::vector<uint8_t> payload_parsed{parsed.begin() + i, parsed.end()};
    EXPECT_EQ(payload_parsed, payload_.ParseArg());
}

class DltStringTest : public ::testing::TestWithParam<
                        std::string> {
};

INSTANTIATE_TEST_SUITE_P(DltStringTestParemeters, DltStringTest,
::testing::Values(
    "",
    "abc",
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
));

TEST_P(DltStringTest, DLT_STRING_PARSE) {
    std::string log = GetParam();
    simba::dlt::data::DltString test_string(log);
    const auto p_length = static_cast<uint16_t>(log.size()) + 1;
    std::vector<uint8_t> expected{0x00, 0x00, 0x82,0x00, 
     static_cast<uint8_t>(p_length >> 8U), static_cast<uint8_t>(p_length & 0xFF)};
    for (char ch : log) {
        expected.push_back(static_cast<uint8_t>(ch));
    }
    expected.push_back(0x00);
    EXPECT_EQ(expected, test_string.ParseArg());
}