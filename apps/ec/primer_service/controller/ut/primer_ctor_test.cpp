/**
 * @copyright Copyright (c) 2026
 */
// F-06: before Initialize() the controller must hold the default igniter pin and pulse time (not an empty config).
#include "gtest/gtest.h"

#define private public  // NOLINT
#include "apps/ec/primer_service/controller/primer_controller.hpp"
#undef private

TEST(PrimerCtorTest, ConstructorStartsFromTheDefaultConfig) {
  // leaked on purpose: the destructor of the real GPIO socket is not safe in a unit-test process
  auto* controller = new srp::primer::PrimerController;
  ASSERT_EQ(controller->config_.pins.size(), 1U);
  EXPECT_EQ(controller->config_.pins[0], 13);
  EXPECT_EQ(controller->config_.active_time_ms, 2500);
}

using srp::primer::PrimerState_t;

TEST(PrimerCtorTest, EnablePrimerWithoutPinsIsRejected) {
  auto* controller = new srp::primer::PrimerController;
  controller->primerState = PrimerState_t::kCONNECTED;  // verification passed, only the pin list is empty
  controller->config_.pins.clear();
  EXPECT_FALSE(controller->EnablePrimer(false));
}
