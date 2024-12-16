/**
 * @file condition_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/common/condition.h"
#include <stop_token>
#include <gtest/gtest.h>
#include <thread>  // NOLINT
#include <chrono>  // NOLINT

using namespace simba::core::condition;  // NOLINT

TEST(ConditionTest, WaitForCompletesWithoutStopRequest) {
    std::stop_source stop_source;
    auto start = std::chrono::steady_clock::now();

    wait_for(std::chrono::milliseconds(100), stop_source.get_token());

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_GE(elapsed, 100);
}

TEST(ConditionTest, WaitForStopsOnStopRequest) {
    std::stop_source stop_source;

    auto start = std::chrono::steady_clock::now();
    std::jthread stopper([&stop_source]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        stop_source.request_stop();
    });

    wait_for(std::chrono::seconds(5), stop_source.get_token());

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_LT(elapsed, 5000);
    EXPECT_GE(elapsed, 50);

    stopper.join();
}

TEST(ConditionTest, WaitStopsOnStopRequest) {
    std::stop_source stop_source;

    auto start = std::chrono::steady_clock::now();
    std::jthread stopper([&stop_source]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        stop_source.request_stop();
    });

    wait(stop_source.get_token());

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    EXPECT_LT(elapsed, 5000);
    EXPECT_GE(elapsed, 50);

    stopper.join();
}

