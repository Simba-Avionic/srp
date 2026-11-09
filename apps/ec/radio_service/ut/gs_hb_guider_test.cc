/**
 * @file gs_hb_guider_test.cc
 * @brief OnNewHbCallback mierzy odstep od poprzedniego heartbeatu (F-43)
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <chrono>  // NOLINT
#include <thread>  // NOLINT

#include "gtest/gtest.h"
#include "apps/ec/radio_service/gs_hb_guider.hpp"

namespace srp {
namespace apps {
namespace radio {

TEST(GsHbGuardTest, IntervalIsMeasuredBetweenConsecutiveHeartbeats) {
  GSHeartbeatGuard guard;
  EXPECT_EQ(guard.LastIntervalMs(), 0);
  guard.OnNewHbCallback();  // pierwszy - tylko uzbraja
  EXPECT_EQ(guard.LastIntervalMs(), 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(120));
  guard.OnNewHbCallback();
  EXPECT_GE(guard.LastIntervalMs(), 100);
  EXPECT_LT(guard.LastIntervalMs(), 2000);
}

}  // namespace radio
}  // namespace apps
}  // namespace srp
