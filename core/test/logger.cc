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
