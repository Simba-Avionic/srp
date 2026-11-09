/**
 * @copyright Copyright (c) 2026
 */
// F-30: a lost GS link reacts only before LAUNCH; from LAUNCH on the guard does nothing.
#include <gtest/gtest.h>

#include "apps/ec/radio_service/gs_hb_guider.hpp"

using srp::apps::radio::GSHeartbeatGuard;
using srp::core::rocketState::RocketState_t;

namespace {
constexpr int64_t kMin = 60 * 1000;
constexpr int64_t kConnLost = 2 * kMin;
constexpr int64_t kAbort = 15 * kMin;
}  // namespace

TEST(GsHbGuard, FlightPhaseCoversLaunchToTouchdown) {
  for (auto s : {RocketState_t::LAUNCH, RocketState_t::FLIGHT, RocketState_t::APOGEE, RocketState_t::FIRST_PARACHUTE,
                 RocketState_t::SECOND_PARACHUTE, RocketState_t::DROP, RocketState_t::TOUCHDOWN}) {
    EXPECT_TRUE(GSHeartbeatGuard::IsFlightPhase(s));
  }
  for (auto s : {RocketState_t::INIT, RocketState_t::DISARM, RocketState_t::ARM, RocketState_t::CONNECTION_LOST,
                 RocketState_t::ABORT}) {
    EXPECT_FALSE(GSHeartbeatGuard::IsFlightPhase(s));
  }
}

TEST(GsHbGuard, BeforeLaunchTheTimeoutsWork) {
  for (auto s : {RocketState_t::INIT, RocketState_t::DISARM, RocketState_t::ARM}) {
    EXPECT_FALSE(GSHeartbeatGuard::Evaluate(s, kConnLost - 1).has_value());
    EXPECT_EQ(GSHeartbeatGuard::Evaluate(s, kConnLost), RocketState_t::CONNECTION_LOST);
    EXPECT_EQ(GSHeartbeatGuard::Evaluate(s, kAbort - 1), RocketState_t::CONNECTION_LOST);
    EXPECT_EQ(GSHeartbeatGuard::Evaluate(s, kAbort), RocketState_t::ABORT);
  }
  // CONNECTION_LOST escalates to ABORT after the long timeout
  EXPECT_EQ(GSHeartbeatGuard::Evaluate(RocketState_t::CONNECTION_LOST, kAbort), RocketState_t::ABORT);
}

TEST(GsHbGuard, FromLaunchOnNothingHappens) {
  for (auto s : {RocketState_t::LAUNCH, RocketState_t::FLIGHT, RocketState_t::APOGEE, RocketState_t::FIRST_PARACHUTE,
                 RocketState_t::SECOND_PARACHUTE, RocketState_t::DROP, RocketState_t::TOUCHDOWN}) {
    EXPECT_FALSE(GSHeartbeatGuard::Evaluate(s, kConnLost).has_value()) << "F-30: reaction during the flight";
    EXPECT_FALSE(GSHeartbeatGuard::Evaluate(s, kAbort).has_value()) << "F-30: ABORT during the flight";
    EXPECT_FALSE(GSHeartbeatGuard::Evaluate(s, 100 * kAbort).has_value());
  }
}
