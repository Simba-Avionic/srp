/**
 * @file logger.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include <vector>

#include "logger/console_logger.h"

TEST(Logger, printTest) {
  simba::core::logger::ConsoleLogger logger{};
  EXPECT_EQ(logger.Debug("log1"), simba::core::ErrorCode::kOk);
  EXPECT_EQ(logger.Info("log1"), simba::core::ErrorCode::kOk);
  EXPECT_EQ(logger.Warning("log1"), simba::core::ErrorCode::kOk);
  EXPECT_EQ(logger.Error("log1"), simba::core::ErrorCode::kOk);
}
