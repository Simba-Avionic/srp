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
                                "1", "08", "2.00", "120.5", "M", "32.8", "M", "", "*49"}
        )
    )
);

TEST(NMEAParser, BasicTest) {
    std::string val = "$GNGGA,182658.40,5421.08022,N,01835.23691,E,1,08,2.00,120.5,M,32.8,M,,*49";
    auto res_opt = srp::core::Nmea::Parse(val);
    ASSERT_TRUE(res_opt.has_value());
    auto res = res_opt.value();
    EXPECT_EQ(res.timestamp, 182658.40);
    EXPECT_EQ(res.latitude, 5421.08022);
    EXPECT_EQ(res.latitude_dir, 'N');
    EXPECT_EQ(res.longitude, 1835.23691);
    EXPECT_EQ(res.longitude_dir, 'E');
    EXPECT_EQ(res.satellite_nr, 8);
    EXPECT_EQ(res.HDOP, 2.0);
    EXPECT_EQ(res.height, 120.5);
}
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
