/**
 * @file gps_app_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <gtest/gtest.h>
#include "apps/fc/gps_service/gps_app.hpp"
#include "core/uart/mock_uart_driver.hpp"

using ::testing::Return;

TEST(GPSAPPTEST, InitializeTestNoUart) {
  const std::map<ara::core::StringView, ara::core::StringView> map;
  srp::apps::GPSApp app;
  EXPECT_EQ(app.Initialize(map), 1);
}

// NO WORK UNTIL COM HAS MOCK
// TEST(GPSAPPTEST, InitializeTestMock) {
//   auto uart_mock_ = std::make_unique<MockUartDriver>();
//   EXPECT_CALL(*uart_mock_, Open(::testing::_, ::testing::_))
//       .Times(1)
//       .WillOnce(::testing::Return(true));
//   const std::map<ara::core::StringView, ara::core::StringView> map;
//   srp::apps::GPSApp app;
//   auto uart_ = std::unique_ptr<srp::core::uart::IUartDriver>(uart_mock_.get());
//   app.Init(std::move(uart_));
//   EXPECT_EQ(app.Initialize(map), 0);
// }

struct GPSDataTestParams {
    srp::core::GPS_DATA_T input_data;
    float expected_latitude;
    float expected_longitude;
};

class GPSGetSomeIPDataTest : public ::testing::TestWithParam<GPSDataTestParams> {};

TEST_P(GPSGetSomeIPDataTest, ValidateGPSData) {
    auto params = GetParam();
    auto res = srp::apps::GPSApp::GetSomeIPData(params.input_data);
    EXPECT_FLOAT_EQ(res.latitude, params.expected_latitude);
    EXPECT_FLOAT_EQ(res.longitude, params.expected_longitude);
}

// Parametry testów
INSTANTIATE_TEST_SUITE_P(
    GPSDataTests,
    GPSGetSomeIPDataTest,
    ::testing::Values(
        GPSDataTestParams{
            srp::core::GPS_DATA_T{12.2, 12.2, 'N', 11.1, 'E', 12, 11.2f, 13.2f},
            12.2f, 11.1f
        },
        GPSDataTestParams{
            srp::core::GPS_DATA_T{12.2, 12.2, 'S', 11.1, 'W', 12, 11.2f, 13.2f},
            -12.2f, -11.1f
        },
        GPSDataTestParams{
            srp::core::GPS_DATA_T{0, 0, 'S', 0, 'W', 12, 11.2f, 13.2f},
            -0, -0
        }
    )
);


TEST(GPSAppTest, ParseGPSData_InvalidData_ReturnsNullopt) {
    std::vector<char> invalidData{'$', 'G', 'P', 'I', 'N', 'V', ',', '1', '2', '3', ',', 'N'};

    auto result = srp::apps::GPSApp::ParseGPSData(invalidData);

    ASSERT_FALSE(result.has_value());
}

TEST(GPSAppTest, ParseGPSData_EmptyData_ReturnsNullopt) {
    std::vector<char> emptyData;

    auto result = srp::apps::GPSApp::ParseGPSData(emptyData);

    ASSERT_FALSE(result.has_value());
}

TEST(GPSAppTest, ParseGPSData_IncompleteGNGGA_ReturnsNullopt) {
    std::vector<char> incompleteData{
        '$', 'G', 'N', 'G', 'G', 'A', ',', '1', '2', '3', '4', '5', '6', ',', '4'
    };

    auto result = srp::apps::GPSApp::ParseGPSData(incompleteData);

    ASSERT_FALSE(result.has_value());
}

TEST(GPSAppTest, ParseGPSData_ZeroSatellites_ReturnsNullopt) {
    std::vector<char> zeroSatellitesData{
        '$', 'G', 'N', 'G', 'G', 'A', ',', '1', '2', '3', '4', '5', '6', ',', '4', '5', '6', '7', '.', '8', ',', 'N',
        ',', '1', '2', '3', '4', '.', '9', ',', 'E', ',', '0', ',', '1', '.', '5', ',', '5', '0', '.', '1', ','
    };

    auto result = srp::apps::GPSApp::ParseGPSData(zeroSatellitesData);

    ASSERT_FALSE(result.has_value());
}

// TEST(GPSAppTest, ParseGPSData) {
//     std::string str = "$GNGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
//     std::vector<char> vec(str.begin(), str.end());
//     auto res = srp::apps::GPSApp::ParseGPSData(vec);
//     EXPECT_TRUE(res.has_value());
//     EXPECT_NEAR(res.value().latitude, 4807.038, 0.001);
//     EXPECT_NEAR(res.value().longitude, 1131.000, 0.001);
// }
// TEST(GPSAppTest, ParseGPSData2) {
//     std::string str = "$GNGGA,123519,4807.038,S,01131.000,W,1,08,0.9,545.4,M,46.9,M,,*47";
//     std::vector<char> vec(str.begin(), str.end());
//     auto res = srp::apps::GPSApp::ParseGPSData(vec);
//     EXPECT_TRUE(res.has_value());
//     EXPECT_NEAR(res.value().latitude, -4807.038, 0.001);
//     EXPECT_NEAR(res.value().longitude, -1131.000, 0.001);
// }

struct GPSDataParam {
    std::string input;      // Wejściowe dane NMEA
    double expected_lat;     // Oczekiwana szerokość geograficzna
    double expected_lon;     // Oczekiwana długość geograficzna
};

class GPSAppTest : public ::testing::TestWithParam<GPSDataParam> {};

TEST_P(GPSAppTest, ParseGPSData) {
    const auto& param = GetParam();
    std::vector<char> vec(param.input.begin(), param.input.end());
    auto res = srp::apps::GPSApp::ParseGPSData(vec);

    EXPECT_TRUE(res.has_value());
    EXPECT_NEAR(res.value().latitude, param.expected_lat, 0.001);
    EXPECT_NEAR(res.value().longitude, param.expected_lon, 0.001);
}

INSTANTIATE_TEST_SUITE_P(
    GPSAppTests,
    GPSAppTest,
    ::testing::Values(
        GPSDataParam{
            "$GNGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47",
            4807.038, 1131.000
        },
        GPSDataParam{
            "$GNGGA,123519,4807.038,S,01131.000,W,1,08,0.9,545.4,M,46.9,M,,*47",
            -4807.038, -1131.000
        },
        GPSDataParam{
            "$GNGGA,223519,3745.123,N,12203.456,W,1,05,1.0,10.0,M,50.0,M,,*5C",
            3745.123, -12203.456
        }
    )
);
