/**
 * @file shm_buffor_skeleton_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <thread>  // NOLINT
#include <cstring>
#include "bindings/common/shm/shm_buffor_skeleton.h"

class ShmBufforSkeletonTest : public ::testing::Test {
 protected:
    // Unique instance specifier for each test
    ara::core::InstanceSpecifier createUniqueInstanceSpec() {
        static int counter = 0;
        std::string uniqueName = "/test_shm_buffor_" + std::to_string(counter++);
        return ara::core::InstanceSpecifier(uniqueName);
    }
};

// Test OfferService with valid parameters
TEST_F(ShmBufforSkeletonTest, OfferServiceSuccess) {
    constexpr std::size_t BUFFER_SIZE = 1024;
    auto instanceSpec = createUniqueInstanceSpec();
    ara::com::shm::ShmBufforSkeleton<BUFFER_SIZE> skeleton(instanceSpec);
    // Test OfferService
    auto result = skeleton.OfferService();
    EXPECT_TRUE(result.HasValue());
}

// Test multiple offers of the same service
TEST_F(ShmBufforSkeletonTest, MultipleOfferService) {
    constexpr std::size_t BUFFER_SIZE = 512;
    auto instanceSpec = createUniqueInstanceSpec();
    // First offer
    {
        ara::com::shm::ShmBufforSkeleton<BUFFER_SIZE> skeleton1(instanceSpec);
        auto result1 = skeleton1.OfferService();
        EXPECT_TRUE(result1.HasValue());
    }
    // Second offer (should overwrite previous)
    {
        ara::com::shm::ShmBufforSkeleton<BUFFER_SIZE> skeleton2(instanceSpec);
        auto result2 = skeleton2.OfferService();
        EXPECT_TRUE(result2.HasValue());
    }
}

// Test GetNewSamplesPointer functionality
TEST_F(ShmBufforSkeletonTest, GetNewSamplesPointer) {
    constexpr std::size_t BUFFER_SIZE = 256;
    auto instanceSpec = createUniqueInstanceSpec();
    ara::com::shm::ShmBufforSkeleton<BUFFER_SIZE> skeleton(instanceSpec);
    // Offer service first
    ASSERT_TRUE(skeleton.OfferService().HasValue());
    // Get pointer
    uint8_t* ptr = skeleton.GetNewSamplesPointer();
    ASSERT_NE(ptr, nullptr);
    // Write some test data
    const char* testData = "Hello, Shared Memory!";
    std::memcpy(ptr, testData, std::strlen(testData) + 1);
    // Verify data
    EXPECT_STREQ(reinterpret_cast<const char*>(ptr), testData);
}

// Test with different buffer sizes
TEST_F(ShmBufforSkeletonTest, DifferentBufferSizes) {
    // Test multiple buffer sizes
    {
        ara::com::shm::ShmBufforSkeleton<128> skeleton1(createUniqueInstanceSpec());
        EXPECT_TRUE(skeleton1.OfferService().HasValue());
    }
    {
        ara::com::shm::ShmBufforSkeleton<1024> skeleton2(createUniqueInstanceSpec());
        EXPECT_TRUE(skeleton2.OfferService().HasValue());
    }
    {
        ara::com::shm::ShmBufforSkeleton<4096> skeleton3(createUniqueInstanceSpec());
        EXPECT_TRUE(skeleton3.OfferService().HasValue());
    }
}

// Multithreaded access test
TEST_F(ShmBufforSkeletonTest, MultithreadedAccess) {
    constexpr std::size_t BUFFER_SIZE = 1024;
    auto instanceSpec = createUniqueInstanceSpec();
    ara::com::shm::ShmBufforSkeleton<BUFFER_SIZE> skeleton(instanceSpec);
    ASSERT_TRUE(skeleton.OfferService().HasValue());
    std::atomic<bool> thread1Done{false};
    std::atomic<bool> thread2Done{false};
    // Thread 1: Write data
    std::thread writer([&]() {
        uint8_t* ptr = skeleton.GetNewSamplesPointer();
        const char* testData = "Multithreaded Test";
        std::memcpy(ptr, testData, std::strlen(testData) + 1);
        thread1Done = true;
    });
    // Thread 2: Read data
    std::thread reader([&]() {
        while (!thread1Done) {
            std::this_thread::yield();
        }
        uint8_t* ptr = skeleton.GetNewSamplesPointer();
        const char* expectedData = "Multithreaded Test";
        EXPECT_STREQ(reinterpret_cast<const char*>(ptr), expectedData);
        thread2Done = true;
    });
    // Wait for threads to complete
    writer.join();
    reader.join();
    EXPECT_TRUE(thread1Done);
    EXPECT_TRUE(thread2Done);
}

// Resource cleanup test
TEST_F(ShmBufforSkeletonTest, CleanupTest) {
    constexpr std::size_t BUFFER_SIZE = 256;
    auto instanceSpec = createUniqueInstanceSpec();
    {
        // Scope block to ensure destructor is called
        ara::com::shm::ShmBufforSkeleton<BUFFER_SIZE> skeleton(instanceSpec);
        ASSERT_TRUE(skeleton.OfferService().HasValue());
    }
    // Try to open the shared memory - it should fail or be recreatable
    ara::com::shm::ShmBufforSkeleton<BUFFER_SIZE> newSkeleton(instanceSpec);
    auto result = newSkeleton.OfferService();
    EXPECT_TRUE(result.HasValue());
}
