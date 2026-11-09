/**
 * @copyright Copyright (c) 2026
 */
// F-01: LAUNCH -> DISARM is allowed (cancelled launch, DISARM from GS during the sequence), LAUNCH -> ARM is not.
#include <gtest/gtest.h>
#include "core/rocket_machine_state/rocket_state.hpp"

using namespace srp::core::rocketState;  // NOLINT

namespace {
void ToLaunch(RocketStateController* c) {
  c->RegisterRequirementsCallback([](RocketState_t) { return true; });
  c->RegisterOnStateChangeCallback([](RocketState_t) {});
  ASSERT_TRUE(c->SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c->SetState(RocketState_t::ARM));
  ASSERT_TRUE(c->SetState(RocketState_t::LAUNCH));
}
}  // namespace

TEST(RocketStateLaunchDisarm, LaunchToDisarmAllowed) {
  RocketStateController c;
  ToLaunch(&c);
  EXPECT_TRUE(c.SetState(RocketState_t::DISARM));
  EXPECT_EQ(c.GetState(), RocketState_t::DISARM);
}

TEST(RocketStateLaunchDisarm, LaunchToArmRejected) {
  RocketStateController c;
  ToLaunch(&c);
  EXPECT_FALSE(c.SetState(RocketState_t::ARM));
  EXPECT_EQ(c.GetState(), RocketState_t::LAUNCH);
}
