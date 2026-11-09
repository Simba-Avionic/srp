/**
 * @copyright Copyright (c) 2025
 */
// Audit tests (bugs): assert the CORRECT behaviour, RED on the current code.
#include <gtest/gtest.h>
#include <chrono>  // NOLINT
#include <thread>  // NOLINT
#include "core/rocket_machine_state/rocket_state.hpp"

using namespace srp::core::rocketState;  // NOLINT
using Clock = std::chrono::steady_clock;

/**
 * F-04 (wysoka) core/rocket_machine_state/rocket_state.cpp:90-115.
 * SetState() holds mtx_ while it runs the per-state callback (engine_app OnDisarm does blocking IPC).
 * Expected: SetState(ABORT) from another thread returns quickly (<500 ms) even while a DISARM
 * callback is still running. Actual: it waits for the whole callback (here ~2 s).
 */
TEST(AuditBugs, F04_AbortBlockedByRunningStateCallback) {
  RocketStateController c;
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  c.RegisterCallback(RocketState_t::DISARM, [] { std::this_thread::sleep_for(std::chrono::seconds(2)); });
  std::thread a([&] { c.SetState(RocketState_t::DISARM); });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  auto t0 = Clock::now();
  c.SetState(RocketState_t::ABORT);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
  a.join();
  EXPECT_LT(ms, 500) << "F-04: SetState(ABORT) blocked " << ms
                     << " ms by a running DISARM callback (rocket_state.cpp:90-115, mtx_ held during callback)";
}

/**
 * F-04b: GetState() has the same blocking (rocket_state.cpp:80-83 vs 90-115).
 */
TEST(AuditBugs, F04_GetStateBlockedByRunningStateCallback) {
  RocketStateController c;
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  c.RegisterCallback(RocketState_t::DISARM, [] { std::this_thread::sleep_for(std::chrono::seconds(2)); });
  std::thread a([&] { c.SetState(RocketState_t::DISARM); });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  auto t0 = Clock::now();
  c.GetState();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
  a.join();
  EXPECT_LT(ms, 500) << "F-04: GetState() blocked " << ms << " ms (rocket_state.cpp:80-83, mtx_ held in SetState)";
}

/**
 * F-30 (niska) core/rocket_machine_state/rocket_state.cpp:23-36 / rocket_state.hpp:146.
 * Comment says connection loss leads to ABORT, but LAUNCH/FLIGHT -> CONNECTION_LOST is rejected, so a lost
 * link during flight has no defined transition. Expected: SetState(CONNECTION_LOST) accepted from LAUNCH.
 */
TEST(AuditBugs, F30_ConnectionLostRejectedInLaunch) {
  RocketStateController c;
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  ASSERT_TRUE(c.SetState(RocketState_t::LAUNCH));
  EXPECT_TRUE(c.SetState(RocketState_t::CONNECTION_LOST))
      << "F-30: CONNECTION_LOST not allowed from LAUNCH (rocket_state.cpp:23-36), link loss in flight unhandled";
}
