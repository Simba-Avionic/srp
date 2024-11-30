/**
 * @file test_result.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include "ara/core/result.h"
#include "ara/core/error_code.h"

using namespace ara::core;  // NOLINT

TEST(ResultTest, ResultFromValue) {
    Result<int> result = Result<int>::FromValue(42);
    ASSERT_TRUE(result.HasValue());
    EXPECT_EQ(result.Value(), 42);
}
