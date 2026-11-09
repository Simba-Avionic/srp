/**
 * @copyright Copyright (c) 2026
 */
// F-04: state callbacks run outside the controller mutex.
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>  // NOLINT
#include <mutex>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <vector>
#include "core/rocket_machine_state/rocket_state.hpp"

using namespace srp::core::rocketState;  // NOLINT
using Clock = std::chrono::steady_clock;

namespace {
long Ms(Clock::time_point t0) {  // NOLINT
  return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
}
}  // namespace

// SetState(ABORT) from another thread must not wait for a running DISARM callback (here ~2 s).
TEST(StateLocking, AbortNotBlockedByRunningStateCallback) {
  RocketStateController c;
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  c.RegisterCallback(RocketState_t::DISARM, [] { std::this_thread::sleep_for(std::chrono::seconds(2)); });
  std::thread a([&] { c.SetState(RocketState_t::DISARM); });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  auto t0 = Clock::now();
  c.SetState(RocketState_t::ABORT);
  const auto ms = Ms(t0);
  a.join();
  EXPECT_LT(ms, 500) << "F-04: SetState(ABORT) blocked " << ms << " ms by a running DISARM callback";
}

TEST(StateLocking, GetStateNotBlockedByRunningStateCallback) {
  RocketStateController c;
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  c.RegisterCallback(RocketState_t::DISARM, [] { std::this_thread::sleep_for(std::chrono::seconds(2)); });
  std::thread a([&] { c.SetState(RocketState_t::DISARM); });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  auto t0 = Clock::now();
  c.GetState();
  const auto ms = Ms(t0);
  a.join();
  EXPECT_LT(ms, 500) << "F-04: GetState() blocked " << ms << " ms";
}

// a callback may call GetState() / SetState() itself (before: deadlock on the non-recursive mutex)
TEST(StateLocking, CallbackCanCallBackIntoController) {
  RocketStateController c;
  std::atomic<int> seen{-1};
  c.RegisterCallback(RocketState_t::DISARM, [&] { seen = c.GetState(); });
  c.RegisterCallback(RocketState_t::ARM, [&] { c.SetState(RocketState_t::ABORT); });
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  EXPECT_EQ(seen.load(), static_cast<int>(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  EXPECT_EQ(c.GetState(), RocketState_t::ABORT);
}

// order of events for one SetState: state change, state callback, on-change callback
TEST(StateLocking, EventOrderIsKept) {
  RocketStateController c;
  std::vector<std::string> events;
  c.RegisterCallback(RocketState_t::DISARM, [&] { events.push_back("state:" + std::to_string(c.GetState())); });
  c.RegisterOnStateChangeCallback([&](RocketState_t s) { events.push_back("change:" + std::to_string(s)); });
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_EQ(events.size(), 2u);
  EXPECT_EQ(events[0], "state:1");
  EXPECT_EQ(events[1], "change:1");
}

// two concurrent transitions: the slower (older) one finishes last, but the last published state is the current one
TEST(StateLocking, LastPublishedStateIsCurrent) {
  RocketStateController c;
  ASSERT_TRUE(c.SetState(RocketState_t::DISARM));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));
  std::mutex mtx;
  std::vector<RocketState_t> published;
  c.RegisterOnStateChangeCallback([&](RocketState_t s) {
    std::lock_guard<std::mutex> lock(mtx);
    published.push_back(s);
  });
  c.RegisterCallback(RocketState_t::DISARM, [] { std::this_thread::sleep_for(std::chrono::milliseconds(500)); });
  std::thread a([&] { c.SetState(RocketState_t::DISARM); });
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  ASSERT_TRUE(c.SetState(RocketState_t::ARM));  // DISARM -> ARM while the DISARM callback is still running
  a.join();
  ASSERT_FALSE(published.empty());
  EXPECT_EQ(published.back(), RocketState_t::ARM);
  EXPECT_EQ(c.GetState(), RocketState_t::ARM);
}
