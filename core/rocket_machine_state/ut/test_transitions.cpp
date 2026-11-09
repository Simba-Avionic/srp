/**
 * @copyright Copyright (c) 2026
 */
// Regression tests for the allowed state transitions (F-02: ABORT is rejected from LAUNCH and FLIGHT,
// LAUNCH -> FLIGHT only, FLIGHT -> ARM allowed for static tests).
#include <gtest/gtest.h>
#include "core/rocket_machine_state/rocket_state.hpp"

using namespace srp::core::rocketState;  // NOLINT

namespace {
RocketStateController* MakeController(RocketStateController* c, int* abort_calls) {
  c->RegisterRequirementsCallback([](RocketState_t) { return true; });
  c->RegisterOnStateChangeCallback([](RocketState_t) {});
  c->RegisterCallback(RocketState_t::ABORT, [abort_calls] { ++*abort_calls; });
  return c;
}
}  // namespace

TEST(RocketStateTransitions, AbortRejectedInLaunch) {
  RocketStateController c;
  int aborts = 0;
  MakeController(&c, &aborts);
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  ASSERT_TRUE(c.SetState(RocketState_t::LAUNCH));
  EXPECT_FALSE(c.SetState(RocketState_t::ABORT));
  EXPECT_EQ(c.GetState(), RocketState_t::LAUNCH);
  EXPECT_EQ(aborts, 0) << "OnAbort callback must not run for a rejected transition";
}

TEST(RocketStateTransitions, AbortRejectedInFlight) {
  RocketStateController c;
  int aborts = 0;
  MakeController(&c, &aborts);
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  ASSERT_TRUE(c.SetState(RocketState_t::LAUNCH));
  ASSERT_TRUE(c.SetState(RocketState_t::FLIGHT));
  EXPECT_FALSE(c.SetState(RocketState_t::ABORT));
  EXPECT_EQ(c.GetState(), RocketState_t::FLIGHT);
  EXPECT_EQ(aborts, 0);
  EXPECT_TRUE(c.SetState(RocketState_t::APOGEE));
}

TEST(RocketStateTransitions, AbortStillAllowedBeforeLaunch) {
  RocketStateController c;
  int aborts = 0;
  MakeController(&c, &aborts);
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  EXPECT_TRUE(c.SetState(RocketState_t::ABORT));
  EXPECT_EQ(aborts, 1);
  EXPECT_TRUE(c.SetState(RocketState_t::DISARM));
}

TEST(RocketStateTransitions, LaunchAllowsFlightAndDisarmOnly) {
  RocketStateController c;
  int aborts = 0;
  MakeController(&c, &aborts);
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  ASSERT_TRUE(c.SetState(RocketState_t::LAUNCH));
  EXPECT_FALSE(c.SetState(RocketState_t::ARM));
  EXPECT_EQ(c.GetState(), RocketState_t::LAUNCH);
  EXPECT_TRUE(c.SetState(RocketState_t::FLIGHT));
}

TEST(RocketStateTransitions, FlightToArmAllowedForStaticTests) {
  RocketStateController c;
  int aborts = 0;
  MakeController(&c, &aborts);
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  ASSERT_TRUE(c.SetState(RocketState_t::LAUNCH));
  ASSERT_TRUE(c.SetState(RocketState_t::FLIGHT));
  EXPECT_TRUE(c.SetState(RocketState_t::ARM));
}
