/**
 * @file timestamp_driver_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "core/timestamp/timestamp_driver.hpp"
#include <gtest/gtest.h>
#include <thread>  // NOLINT
#include <chrono>  // NOLINT
#include <iostream>

namespace srp {
namespace core {
namespace timestamp {
constexpr auto KBasicCorrection = 0.1;
class TimestampMasterTest : public ::testing::Test {
 protected:
  TimestampMaster master;
  void SetUp() override {
    master.Init();
  }
};

TEST_F(TimestampMasterTest, GetNewTimeStampReturnsElapsedTime) {
    constexpr auto K1 = 100;
    constexpr auto K2 = 100;
    constexpr auto K3 = 1000;
    std::this_thread::sleep_for(std::chrono::milliseconds(K1));
    int64_t timestamp = master.GetNewTimeStamp();
    std::this_thread::sleep_for(std::chrono::milliseconds(K2));
    int64_t timestamp2 = master.GetNewTimeStamp();
    std::this_thread::sleep_for(std::chrono::milliseconds(K3));
    int64_t timestamp3 = master.GetNewTimeStamp();

    EXPECT_NEAR(timestamp, K1, K1 * KBasicCorrection);
    EXPECT_NEAR(timestamp2, K1 + K2, (K1 + K2) * KBasicCorrection);
    EXPECT_NEAR(timestamp3, K1 + K2 + K3, (K1 + K2 + K3) * KBasicCorrection);
}

TEST_F(TimestampMasterTest, CorrectStartPointAdjustsStartTime) {
    constexpr auto K1 = 50;
    int64_t initial_timestamp = master.GetNewTimeStamp();
    master.CorrectStartPoint(K1);  // Korekcja o 50 ms
    int64_t adjusted_timestamp = master.GetNewTimeStamp();

    EXPECT_NEAR(adjusted_timestamp, initial_timestamp + K1, KBasicCorrection);
}
TEST_F(TimestampMasterTest, InitOffersServiceAndSetsStartTime) {
    EXPECT_NO_THROW(master.Init());  // Sprawdzenie, czy Init nie generuje wyjątków
    int64_t timestamp = master.GetNewTimeStamp();

    EXPECT_GE(timestamp, 0);  // Powinien być dodatni
}

TEST(TimestampIntegratedTest, FirstTest) {
    TimestampMaster master;
    TimestampController slave1;
    TimestampController slave2;
    EXPECT_TRUE(master.Init());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_TRUE(slave1.Init());
    EXPECT_TRUE(slave2.Init());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto t1 = slave1.GetNewTimeStamp();
    auto t2 = slave2.GetNewTimeStamp();
    auto correct_val = master.GetNewTimeStamp();
    ASSERT_TRUE(t1.has_value());
    ASSERT_TRUE(t2.has_value());
    EXPECT_NEAR(correct_val, t1.value(), 70);
    EXPECT_NEAR(correct_val, t2.value(), 70);
}
TEST(TimestampIntegratedTest, SecTest) {
    TimestampMaster master;
    TimestampController slave1;
    EXPECT_TRUE(master.Init());
    EXPECT_TRUE(slave1.Init());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto t1 = slave1.GetNewTimeStamp();
    ASSERT_TRUE(t1.has_value());
    EXPECT_NEAR(master.GetNewTimeStamp(), t1.value(), 70);
}
TEST(TimestampTest, Error) {
    TimestampController slave;
    EXPECT_TRUE(slave.Init());
    auto t1 = slave.GetNewTimeStamp();
    ASSERT_FALSE(t1.has_value());
}

}  // namespace timestamp
}  // namespace core
}  // namespace srp
