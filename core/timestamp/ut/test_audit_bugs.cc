/**
 * @copyright Copyright (c) 2025
 */
// Audit tests (bugs): assert the CORRECT behaviour, RED on the current code.
#include <gtest/gtest.h>
#include "core/timestamp/timestamp_driver.hpp"

/**
 * F-45 (niska) core/timestamp/timestamp_driver.cpp:29-39. TimestampController::Init loops while i <= kMax_Wait (5), ends
 * with i == 6 and reports failure only for i == 5 -> with no timestamp service for 6 s Init() returns true.
 * Expected: false when the service never appears. Actual: true.
 */
TEST(AuditBugs, F45_TimestampControllerInitSucceedsWithoutService) {
  srp::core::timestamp::TimestampController c;
  EXPECT_FALSE(c.Init())
      << "F-45: Init() returned true although no timestamp service exists (timestamp_driver.cpp:29-39)";
}
