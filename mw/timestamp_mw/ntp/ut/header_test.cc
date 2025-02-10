/**
 * @file header_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "mw/timestamp_mw/ntp/header/header.hpp"


class NtpHeaderTest : public ::testing::Test {
 protected:
  srp::tinyNTP::NtpHeader ntpHeader;
};
TEST_F(NtpHeaderTest, DefaultConstructor) {
    EXPECT_EQ(ntpHeader.GetSettings(), 0);
    EXPECT_EQ(ntpHeader.GetT0Timestamp(), 0);
    EXPECT_EQ(ntpHeader.GetT1Timestamp(), 0);
    EXPECT_EQ(ntpHeader.GetT2Timestamp(), 0);
    EXPECT_EQ(ntpHeader.GetT3Timestamp(), 0);
}

TEST_F(NtpHeaderTest, SetSettings) {
    ntpHeader.SetSettings(3, srp::tinyNTP::NTPMode_t::CLIENT);
    EXPECT_EQ(ntpHeader.GetSettings(), (3 << 3) | srp::tinyNTP::NTPMode_t::CLIENT);
}

TEST_F(NtpHeaderTest, SetAndGetTimestamps) {
    int64_t t0 = 1000000;
    int64_t t1 = 2000000;
    int64_t t2 = 3000000;
    int64_t t3 = 4000000;

    ntpHeader.SetT0Timestamp(t0);
    ntpHeader.SetT1Timestamp(t1);
    ntpHeader.SetT2Timestamp(t2);
    ntpHeader.SetT3Timestamp(t3);

    EXPECT_EQ(ntpHeader.GetT0Timestamp(), t0);
    EXPECT_EQ(ntpHeader.GetT1Timestamp(), t1);
    EXPECT_EQ(ntpHeader.GetT2Timestamp(), t2);
    EXPECT_EQ(ntpHeader.GetT3Timestamp(), t3);
}
TEST_F(NtpHeaderTest, SetBufor) {
    int64_t t0 = 1000000;
    int64_t t1 = 2000000;
    int64_t t2 = 3000000;
    int64_t t3 = 4000000;

    ntpHeader.SetT0Timestamp(t0);
    ntpHeader.SetT1Timestamp(t1);
    ntpHeader.SetT2Timestamp(t2);
    ntpHeader.SetT3Timestamp(t3);
    auto buf = ntpHeader.GetBuffor();
    srp::tinyNTP::NtpHeader test;
    test.SetBuffor(buf);

    EXPECT_EQ(test.GetT0Timestamp(), t0);
    EXPECT_EQ(test.GetT1Timestamp(), t1);
    EXPECT_EQ(test.GetT2Timestamp(), t2);
    EXPECT_EQ(test.GetT3Timestamp(), t3);
}
