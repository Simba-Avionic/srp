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
    EXPECT_FLOAT_EQ(res.lattitude, params.expected_latitude);
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
