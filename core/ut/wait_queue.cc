/**
 * @file wait_queue.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/common/wait_queue.h"
#include <gtest/gtest.h>
#include <optional>
#include <thread>  // NOLINT
#include <stop_token>  // NOLINT

using namespace srp::core;  // NOLINT

TEST(WaitQueueTest, GetNoWait) {
    auto queue = std::make_shared<WaitQueue<int, 10>>();
    std::stop_source source;
    queue->push(10);
    auto res = queue->Get(source.get_token());
    EXPECT_TRUE(queue->IsEmpty());
    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 10);
}

TEST(WaitQueueTest, GetWithoutRemoveNoWait) {
    auto queue = std::make_shared<WaitQueue<int, 10>>();
    std::stop_source source;
    queue->push(10);
    auto res = queue->GetWithoutRemove(source.get_token());
    EXPECT_FALSE(queue->IsEmpty());
    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 10);
}
TEST(WaitQueueTest, GetWithoutRemoveWait) {
    auto queue = std::make_shared<WaitQueue<int, 10>>();
    std::stop_source source;
    std::jthread thread([queue](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        queue->push(10);
    });
    auto res = queue->GetWithoutRemove(source.get_token());
    thread.join();
    EXPECT_FALSE(queue->IsEmpty());
    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 10);
}

TEST(WaitQueueTest, GetWithoutRemoveStopRequest) {
    auto queue = std::make_shared<WaitQueue<int, 10>>();

    std::stop_source stopSource;
    std::stop_token token = stopSource.get_token();
    std::jthread thread([&queue, &token, &stopSource]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        stopSource.request_stop();
    });
    auto res = queue->GetWithoutRemove(token);
    thread.join();
    EXPECT_TRUE(queue->IsEmpty());
    EXPECT_FALSE(res.has_value());
}

TEST(WaitQueueTest, PushAndIsEmpty) {
    WaitQueue<int, 10> queue;
    EXPECT_TRUE(queue.IsEmpty());

    queue.push(42);
    EXPECT_FALSE(queue.IsEmpty());
}

TEST(WaitQueueTest, PushWithMaxSize) {
    constexpr uint16_t max_size = 3;
    WaitQueue<int, max_size> queue;

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));
    EXPECT_TRUE(queue.push(3));

    // Kolejka osiągnęła maksymalny rozmiar, dodanie kolejnego elementu powoduje usunięcie najstarszego
    EXPECT_FALSE(queue.push(4));

    EXPECT_FALSE(queue.IsEmpty());
}

TEST(WaitQueueTest, GetRemovesElement) {
    WaitQueue<int> queue;
    queue.push(10);
    queue.push(20);

    EXPECT_EQ(queue.Get(), 10);
    EXPECT_EQ(queue.Get(), 20);
    EXPECT_TRUE(queue.IsEmpty());
}
TEST(WaitQueueTest, GetElement) {
    WaitQueue<int> queue;
    queue.push(10);
    queue.push(20);
    EXPECT_EQ(queue.Get(), 10);
    EXPECT_FALSE(queue.IsEmpty());
}

TEST(WaitQueueTest, GetWithoutRemoveDoesNotRemoveElement) {
    WaitQueue<int> queue;
    queue.push(10);

    EXPECT_EQ(queue.GetWithoutRemove(), 10);
    EXPECT_FALSE(queue.IsEmpty());
    EXPECT_EQ(queue.Get(), 10);
    EXPECT_TRUE(queue.IsEmpty());
}

TEST(WaitQueueTest, GetBlocksUntilElementAvailable) {
    WaitQueue<int> queue;

    std::thread producer([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        queue.push(42);
    });

    EXPECT_EQ(queue.Get(), 42);
    EXPECT_TRUE(queue.IsEmpty());

    producer.join();
}

TEST(WaitQueueTest, GetWithStopTokenStopsWaiting) {
    WaitQueue<int> queue;
    std::jthread consumer([&queue](std::stop_token stoken) {
        auto result = queue.Get(stoken);
        EXPECT_FALSE(result.has_value());
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    consumer.request_stop();
    consumer.join();
}

TEST(WaitQueueTest, GetWithoutRemoveBlocksUntilElementAvailable) {
    WaitQueue<int> queue;

    std::thread producer([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        queue.push(99);
    });

    EXPECT_EQ(queue.GetWithoutRemove(), 99);
    EXPECT_FALSE(queue.IsEmpty());
    EXPECT_EQ(queue.Get(), 99);
    EXPECT_TRUE(queue.IsEmpty());

    producer.join();
}

TEST(WaitQueueTest, RemoveRemovesElement) {
    WaitQueue<int> queue;
    queue.push(1);
    queue.push(2);

    queue.Remove();
    EXPECT_EQ(queue.Get(), 2);
    EXPECT_TRUE(queue.IsEmpty());
}
