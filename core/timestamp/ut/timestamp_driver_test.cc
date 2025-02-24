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

class TimestampControllerTest : public ::testing::Test {
 protected:
    TimestampController controller;
    TimestampMaster master;

    void SetUp() override {
    master.Init();
    controller.Init();  // Inicjalizacja obiektu
    }
};

TEST_F(TimestampControllerTest, GetNewTimeStampReturnsElapsedTimeInLocalMode) {
    std::optional<int64_t> timestamp1 = controller.GetNewTimeStamp();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::optional<int64_t> timestamp2 = controller.GetNewTimeStamp();

    ASSERT_TRUE(timestamp1.has_value());
    ASSERT_TRUE(timestamp2.has_value());
    EXPECT_GE(timestamp2.value(), timestamp1.value() + 50);
}

class TimestampMasterTest : public ::testing::Test {
 protected:
  TimestampMaster master;
  void SetUp() override {
    master.Init();
  }
};

TEST_F(TimestampMasterTest, GetNewTimeStampReturnsElapsedTime) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int64_t timestamp = master.GetNewTimeStamp();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int64_t timestamp2 = master.GetNewTimeStamp();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    int64_t timestamp3 = master.GetNewTimeStamp();

    EXPECT_NEAR(timestamp, 100, 1);
    EXPECT_NEAR(timestamp2, 200, 1);
    EXPECT_NEAR(timestamp3, 1200, 1);
}

TEST_F(TimestampMasterTest, CorrectStartPointAdjustsStartTime) {
    int64_t initial_timestamp = master.GetNewTimeStamp();
    master.CorrectStartPoint(50);  // Korekcja o 50 ms
    int64_t adjusted_timestamp = master.GetNewTimeStamp();

    EXPECT_LT(adjusted_timestamp, initial_timestamp + 51);
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
    master.Init();
    slave1.Init();
    slave2.Init();
    auto t1 = slave1.GetNewTimeStamp();
    auto t2 = slave2.GetNewTimeStamp();
    ASSERT_TRUE(t1.has_value());
    ASSERT_TRUE(t2.has_value());
    EXPECT_NEAR(master.GetNewTimeStamp(), t1.value(), 2);
    EXPECT_NEAR(master.GetNewTimeStamp(), t2.value(), 2);
}
TEST(TimestampIntegratedTest, SecTest) {
    TimestampMaster master;
    TimestampController slave1;
    master.Init();
    slave1.Init();
    auto t1 = slave1.GetNewTimeStamp();
    ASSERT_TRUE(t1.has_value());
    EXPECT_NEAR(master.GetNewTimeStamp(), t1.value(), 2);
}
TEST(TimestampTest, Error) {
    TimestampController slave;
    slave.Init();
    auto t1 = slave.GetNewTimeStamp();
    ASSERT_FALSE(t1.has_value());
}

}  // namespace timestamp
}  // namespace core
}  // namespace srp
