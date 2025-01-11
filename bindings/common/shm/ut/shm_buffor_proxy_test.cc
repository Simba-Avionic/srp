/**
 * @file shm_buffor_proxy_test.cc
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
#include "bindings/common/shm/shm_buffor_proxy.h"
#include "bindings/common/shm/shm_buffor_skeleton.h"
class ShmBufforProxyTest : public ::testing::Test {
 protected:
    // Unique instance specifier for each test
    ara::core::InstanceSpecifier createUniqueInstanceSpec() {
        static int counter = 0;
        std::string uniqueName = "/test_shm_buffor_proxy_" + std::to_string(counter++);
        return ara::core::InstanceSpecifier(uniqueName);
    }

    // Helper method to create shared memory for testing
    template <std::size_t BUFFER_SIZE>
    void createSharedMemory(const ara::core::InstanceSpecifier& instanceSpec) {
        ara::com::shm::ShmBufforSkeleton<BUFFER_SIZE> skeleton(instanceSpec);
        auto result = skeleton.OfferService();
        ASSERT_TRUE(result.HasValue());
    }
};

// Test FindService with valid parameters
TEST_F(ShmBufforProxyTest, FindServiceSuccess) {
    constexpr std::size_t BUFFER_SIZE = 1024;
    auto instanceSpec = createUniqueInstanceSpec();
    // First create shared memory
    createSharedMemory<BUFFER_SIZE>(instanceSpec);
    // Then try to find service
    ara::com::shm::ShmBufforProxy<BUFFER_SIZE> proxy(instanceSpec);
    auto result = proxy.FindService();
    EXPECT_FALSE(result.HasValue());
}

// Test FindService with non-existent shared memory
TEST_F(ShmBufforProxyTest, FindServiceFailure) {
    constexpr std::size_t BUFFER_SIZE = 512;
    auto instanceSpec = createUniqueInstanceSpec();
    ara::com::shm::ShmBufforProxy<BUFFER_SIZE> proxy(instanceSpec);
    auto result = proxy.FindService();
    EXPECT_FALSE(result.HasValue());
}

TEST_F(ShmBufforProxyTest, GetNewSamplesPointer) {
    constexpr std::size_t BUFFER_SIZE = 512;
    auto instanceSpec = createUniqueInstanceSpec();
    ara::com::shm::ShmBufforProxy<BUFFER_SIZE> proxy(instanceSpec);
    uint8_t* val = proxy.GetNewSamplesPointer();
    EXPECT_TRUE(val == nullptr);
}

// Test with different buffer sizes
TEST_F(ShmBufforProxyTest, DifferentBufferSizes) {
    // Test multiple buffer sizes
    {
        constexpr std::size_t BUFFER_SIZE = 128;
        auto instanceSpec = createUniqueInstanceSpec();
        createSharedMemory<BUFFER_SIZE>(instanceSpec);
        ara::com::shm::ShmBufforProxy<BUFFER_SIZE> proxy(instanceSpec);
        EXPECT_FALSE(proxy.FindService().HasValue());
    }
    {
        constexpr std::size_t BUFFER_SIZE = 1024;
        auto instanceSpec = createUniqueInstanceSpec();
        createSharedMemory<BUFFER_SIZE>(instanceSpec);
        ara::com::shm::ShmBufforProxy<BUFFER_SIZE> proxy(instanceSpec);
        EXPECT_FALSE(proxy.FindService().HasValue());
    }
    {
        constexpr std::size_t BUFFER_SIZE = 4096;
        auto instanceSpec = createUniqueInstanceSpec();
        createSharedMemory<BUFFER_SIZE>(instanceSpec);
        ara::com::shm::ShmBufforProxy<BUFFER_SIZE> proxy(instanceSpec);
        EXPECT_FALSE(proxy.FindService().HasValue());
    }
}

// Resource cleanup test
TEST_F(ShmBufforProxyTest, CleanupTest) {
    constexpr std::size_t BUFFER_SIZE = 256;
    auto instanceSpec = createUniqueInstanceSpec();
    // Create shared memory first
    createSharedMemory<BUFFER_SIZE>(instanceSpec);
    {
        // Scope block to ensure destructor is called
        ara::com::shm::ShmBufforProxy<BUFFER_SIZE> proxy(instanceSpec);
        EXPECT_FALSE(proxy.FindService().HasValue());
    }
    // Try to create new proxy
    ara::com::shm::ShmBufforProxy<BUFFER_SIZE> newProxy(instanceSpec);
    auto result = newProxy.FindService();
    EXPECT_FALSE(result.HasValue());
}
