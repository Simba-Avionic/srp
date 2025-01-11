/**
 * @file shm_proxy_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <pthread.h>
#include "bindings/common/shm/shm_proxy.h"

// Test struct for shared memory
struct TestData {
    int value;
    double floating_point;
};

class ShmProxyTest : public ::testing::Test {
 protected:
    // Unique instance specifier for each test
    ara::core::InstanceSpecifier createUniqueInstanceSpec() {
        static int counter = 0;
        std::string uniqueName = "/test_shm_" + std::to_string(counter++);
        return ara::core::InstanceSpecifier(uniqueName);
    }
    // Helper method to manually create shared memory for testing
    int createSharedMemory(const std::string& name) {
        int shm_fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1) {
            throw std::runtime_error("Failed to create shared memory");
        }
        if (ftruncate(shm_fd, sizeof(typename ara::com::shm::ShmProxy<TestData>)) == -1) {
            close(shm_fd);
            throw std::runtime_error("Failed to truncate shared memory");
        }
        return shm_fd;
    }
};

// Test FindService with valid instance specifier
TEST_F(ShmProxyTest, FindServiceSuccess) {
    // Create a unique instance specifier
    auto instanceSpec = createUniqueInstanceSpec();
    // Create shared memory manually first
    int shm_fd = createSharedMemory(instanceSpec.ToString());
    close(shm_fd);
    // Create ShmProxy
    ara::com::shm::ShmProxy<TestData> proxy(instanceSpec);
    // Test FindService
    auto result = proxy.FindService();
    EXPECT_TRUE(result.HasValue());
}

// Test FindService with non-existent shared memory
TEST_F(ShmProxyTest, FindServiceFailure) {
    // Use a non-existent shared memory name
    ara::core::InstanceSpecifier instanceSpec("/non_existent_shm");
    ara::com::shm::ShmProxy<TestData> proxy(instanceSpec);
    // Test FindService
    auto result = proxy.FindService();
    EXPECT_FALSE(result.HasValue());
}

// Test GetNewSamples without FindService called first
TEST_F(ShmProxyTest, GetNewSamplesWithoutFindService) {
    auto instanceSpec = createUniqueInstanceSpec();
    ara::com::shm::ShmProxy<TestData> proxy(instanceSpec);
    auto result = proxy.GetNewSamples();
    EXPECT_FALSE(result.HasValue());
}

// Test GetNewSamplesPointer functionality
TEST_F(ShmProxyTest, GetNewSamplesPointer) {
    // Create a unique instance specifier
    auto instanceSpec = createUniqueInstanceSpec();
    // Create shared memory manually first
    int shm_fd = createSharedMemory(instanceSpec.ToString());
    close(shm_fd);

    // Create ShmProxy
    ara::com::shm::ShmProxy<TestData> proxy(instanceSpec);
    // Find service first
    ASSERT_TRUE(proxy.FindService().HasValue());
    // Get pointer to shared memory
    auto ptrResult = proxy.GetNewSamplesPointer();
    ASSERT_TRUE(ptrResult.HasValue());
    // Modify shared memory
    auto* dataPtr = ptrResult.Value();
    dataPtr->value = 42;
    dataPtr->floating_point = 3.14159;
    // Retrieve and verify
    auto samples = proxy.GetNewSamples();
    ASSERT_TRUE(samples.HasValue());
    EXPECT_EQ(samples.Value().value, 42);
    EXPECT_DOUBLE_EQ(samples.Value().floating_point, 3.14159);
}

// Multithreaded access test
TEST_F(ShmProxyTest, MultithreadedAccess) {
    // Create a unique instance specifier
    auto instanceSpec = createUniqueInstanceSpec();
    // Create shared memory manually first
    int shm_fd = createSharedMemory(instanceSpec.ToString());
    close(shm_fd);

    // Create ShmProxy
    ara::com::shm::ShmProxy<TestData> proxy(instanceSpec);
    // Find service first
    ASSERT_TRUE(proxy.FindService().HasValue());
    // Thread-safe test
    std::atomic<bool> thread1Done{false};
    std::atomic<bool> thread2Done{false};
    // Thread 1: Write data
    std::thread writer([&]() {
        auto ptrResult = proxy.GetNewSamplesPointer();
        ASSERT_TRUE(ptrResult.HasValue());
        auto* dataPtr = ptrResult.Value();
        dataPtr->value = 100;
        thread1Done = true;
    });
    // Thread 2: Read data
    std::thread reader([&]() {
        while (!thread1Done) {
            std::this_thread::yield();
        }
        auto samples = proxy.GetNewSamples();
        ASSERT_TRUE(samples.HasValue());
        EXPECT_EQ(samples.Value().value, 100);
        thread2Done = true;
    });
    // Wait for threads to complete
    writer.join();
    reader.join();
    EXPECT_TRUE(thread1Done);
    EXPECT_TRUE(thread2Done);
}

// Clean up shared memory after each test
TEST_F(ShmProxyTest, CleanupTest) {
    auto instanceSpec = createUniqueInstanceSpec();
    {
        // Scope block to ensure destructor is called
        ara::com::shm::ShmProxy<TestData> proxy(instanceSpec);
        EXPECT_FALSE(proxy.FindService().HasValue());
    }
    // Try to open the shared memory - it should fail
    int shm_fd = shm_open(instanceSpec.ToString().c_str(), O_RDWR, 0666);
    EXPECT_EQ(shm_fd, -1);
    if (shm_fd != -1) {
        close(shm_fd);
    }
}
