/**
 * @file shm_skeleton_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "bindings/common/shm/shm_skeleton.h"
namespace srp {

class ShmSkeletonTest : public ::testing::Test {
 protected:
    // Helper method to create a unique instance specifier for each test
    ara::core::InstanceSpecifier createUniqueInstanceSpecifier() {
        static int counter = 0;
        return ara::core::InstanceSpecifier("test_shm_" + std::to_string(counter++));
    }

    // Cleanup method to ensure shared memory is properly removed
    void TearDown() override {
        // Attempt to unlink any leftover shared memory segments
        system("ipcs -m | grep $(whoami) | awk '{print $2}' | xargs -I {} ipcrm -m {}");
        system("ipcs -m | grep $(whoami) | awk '{print $2}' | xargs -I {} ipcrm -m {}");
    }
};

// Test default constructor and instance specifier
TEST_F(ShmSkeletonTest, ConstructorTest) {
    auto instanceSpec = createUniqueInstanceSpecifier();
    // Verify constructor accepts instance specifier
    ASSERT_NO_THROW({
        ara::com::shm::ShmSkeleton<int> skeleton(instanceSpec);
    });

    // Verify move and copy constructors are deleted
    ASSERT_FALSE(std::is_copy_constructible<ara::com::shm::ShmSkeleton<int>>::value);
    ASSERT_FALSE(std::is_move_constructible<ara::com::shm::ShmSkeleton<int>>::value);
}

// Test OfferService method
TEST_F(ShmSkeletonTest, OfferServiceTest) {
    auto instanceSpec = createUniqueInstanceSpecifier();
    ara::com::shm::ShmSkeleton<int> skeleton(instanceSpec);

    // First offer service should succeed
    auto result1 = skeleton.OfferService();
    ASSERT_TRUE(result1.HasValue());

    // Second offer service might fail due to existing shared memory segment
    auto result2 = skeleton.OfferService();
    ASSERT_TRUE(result2.HasValue());
}

// Test with different data types
TEST_F(ShmSkeletonTest, DifferentDataTypesTest) {
    auto instanceSpec = createUniqueInstanceSpecifier();
    // Test with double type
    ara::com::shm::ShmSkeleton<double> doubleSkeleton(instanceSpec);
    doubleSkeleton.OfferService();
    auto doubleResult = doubleSkeleton.Send(3.14159);
    ASSERT_TRUE(doubleResult.HasValue());

    // Test with custom struct
    struct TestStruct {
        int x;
        char y;
    };

    auto structInstanceSpec = createUniqueInstanceSpecifier();
    ara::com::shm::ShmSkeleton<TestStruct> structSkeleton(structInstanceSpec);
    structSkeleton.OfferService();
    TestStruct testData{42, 'A'};
    auto structResult = structSkeleton.Send(testData);
    ASSERT_TRUE(structResult.HasValue());
}
}  // namespace srp

