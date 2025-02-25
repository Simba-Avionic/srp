/**
 * @file controler_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "mw/timestamp_mw/ntp/controller/ntp_controller.hpp"
class NtpControllerTest : public ::testing::Test {
 protected:
    srp::tinyNTP::NtpController ntpController;
};

TEST_F(NtpControllerTest, CalculateOffsetTest) {
    int64_t T0 = 1000;
    int64_t T1 = 1500;
    int64_t T2 = 2000;
    int64_t T3 = 2500;

    int64_t expected_offset = 0;  // ((1500 - 1000) + (2000 - 2500)) / 2 = 0
    EXPECT_EQ(ntpController.CalculateOffset(T0, T1, T2, T3), expected_offset);
}

TEST_F(NtpControllerTest, CalculateRoundTripDelayTest) {
    int64_t T0 = 1000;
    int64_t T1 = 1500;
    int64_t T2 = 2000;
    int64_t T3 = 3000;

    int64_t expected_delay = 1500;  // (3000 - 1000) - (2000 - 1500) = 1500
    EXPECT_EQ(ntpController.CalculateRoundTripDelay(T0, T1, T2, T3), expected_delay);
}

TEST_F(NtpControllerTest, GetNowMSTest) {
    auto result = ntpController.GetTimestamp();
    EXPECT_TRUE(result > 0) << "GetNowMS should return a positive timestamp.";
}
