/**
 * @file nmea_tests.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <string>
#include "core/gps/nmea.hpp"
class SplitStringParamTest : public ::testing::TestWithParam<std::tuple<std::string, std::vector<std::string>>> {
};

TEST_P(SplitStringParamTest, SplitStringTest) {
    auto [input, expected] = GetParam();
    auto result = srp::core::Nmea::splitString(input);
    EXPECT_EQ(result, expected);
}

INSTANTIATE_TEST_SUITE_P(
    SplitStringTests,
    SplitStringParamTest,
    ::testing::Values(
        std::make_tuple("182658.40,5421.08022,", std::vector<std::string>{"182658.40", "5421.08022"}),
        std::make_tuple("182658.40,,,", std::vector<std::string>{"182658.40", "", ""}),
        std::make_tuple("182658.40,,5421.08022", std::vector<std::string>{"182658.40", "", "5421.08022"}),
        std::make_tuple("", std::vector<std::string>{}),
        std::make_tuple(
            "182658.40,5421.08022,N,01835.23691,E,1,08,2.00,120.5,M,32.8,M,,*49",
            std::vector<std::string>{"182658.40", "5421.08022", "N", "01835.23691", "E",
                                "1", "08", "2.00", "120.5", "M", "32.8", "M", "", "*49"})
    )
);

struct NMEAParseTestCase {
    std::string input;
    double expected_timestamp;
    double expected_latitude;
    char expected_lat_dir;
    double expected_longitude;
    char expected_long_dir;
    int expected_sat_nr;
    double expected_HDOP;
    double expected_height;
};

class NMEAParserTest : public ::testing::TestWithParam<NMEAParseTestCase> {};

TEST_P(NMEAParserTest, ParsesCorrectly) {
    const auto& param = GetParam();
    auto res_opt = srp::core::Nmea::Parse(param.input);
    ASSERT_TRUE(res_opt.has_value());
    auto res = res_opt.value();

    EXPECT_DOUBLE_EQ(res.timestamp, param.expected_timestamp);
    EXPECT_DOUBLE_EQ(res.latitude, param.expected_latitude);
    EXPECT_EQ(res.latitude_dir, param.expected_lat_dir);
    EXPECT_DOUBLE_EQ(res.longitude, param.expected_longitude);
    EXPECT_EQ(res.longitude_dir, param.expected_long_dir);
    EXPECT_EQ(res.satellite_nr, param.expected_sat_nr);
    EXPECT_NEAR(res.HDOP, param.expected_HDOP, 0.01);
    EXPECT_NEAR(res.height, param.expected_height, 0.01);
}

INSTANTIATE_TEST_SUITE_P(
    NMEAParserTests,
    NMEAParserTest,
    ::testing::Values(
        NMEAParseTestCase{
            "$GNGGA,182658.40,5421.08022,N,01835.23691,E,1,08,2.00,120.5,M,32.8,M,,*49",
            182658.40, 5421.08022, 'N', 1835.23691, 'E', 8, 2.0, 120.5
        },
        NMEAParseTestCase{
            "$GNGGA,120000.00,1234.56789,S,09876.54321,W,1,05,1.50,50.0,M,30.0,M,,*55",
            120000.00, 1234.56789, 'S', 9876.54321, 'W', 5, 1.5, 50.0
        },
        NMEAParseTestCase{
            "$GNGGA,235959.99,0000.00000,N,00000.00000,E,1,10,0.90,10.0,M,20.0,M,,*6A",
            235959.99, 0.0, 'N', 0.0, 'E', 10, 0.9, 10.0
        },
        NMEAParseTestCase{
            "$GNGGA,101010.10,3745.12345,N,12227.12345,W,1,07,1.10,15.3,M,10.0,M,,*5F",
            101010.10, 3745.12345, 'N', 12227.12345, 'W', 7, 1.1, 15.3
        },
        NMEAParseTestCase{
            "$GNGGA,060305.00,5150.12345,S,00007.65432,E,1,06,0.80,5.0,M,12.0,M,,*4A",
            60305.00, 5150.12345, 'S', 7.65432, 'E', 6, 0.8, 5.0
        },
        NMEAParseTestCase{
            "$GNGGA,130707.77,9000.00000,N,18000.00000,E,1,12,0.50,0.0,M,0.0,M,,*4B",
            130707.77, 9000.0, 'N', 18000.0, 'E', 12, 0.5, 0.0
        },
        NMEAParseTestCase{
            "$GNGGA,001122.33,4823.45678,N,01622.98765,E,1,09,0.95,200.0,M,45.0,M,,*61",
            1122.33, 4823.45678, 'N', 1622.98765, 'E', 9, 0.95, 200.0
        },
        NMEAParseTestCase{
            "$GNGGA,151515.15,3456.78901,S,12345.67890,W,1,04,1.75,75.5,M,25.0,M,,*58",
            151515.15, 3456.78901, 'S', 12345.67890, 'W', 4, 1.75, 75.5
        }
    )
);

TEST(NMEAParser, ToStringTest) {
    srp::core::GPS_DATA_T data;
      data.timestamp = 12.2;
      data.latitude = 12.2;
      data.latitude_dir = 'R';
      data.longitude = 11.1;
      data.longitude_dir = 'E';
      data.satellite_nr = 12;
      data.HDOP = 11.2;
      data.height = 13.2;
    EXPECT_EQ(data.to_string(),
          "Timestamp: 12.2, Latitude: 12.2 R, Longitude: 11.1 E, Satellites: 12, HDOP: 11.2, Height: 13.2");
}
