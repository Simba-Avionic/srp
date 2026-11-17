/**
 * @copyright Copyright (c) 2025
 */
// Audit tests (bugs): assert the CORRECT behaviour, RED on the current code.
#include <gtest/gtest.h>
#include "mw/temp/controller/temp_controller.h"

/**
 * F-26 (srednia) mw/temp/controller/temp_controller.cpp:66-68 (Register) and :79-80 (Subscribe).
 * A physical id of the wrong length (EEPROM 0xFF.. gives "28-"+13 chars) makes Subscribe() return {kError}
 * and Register() reports that error code (2) as a valid sensor id.
 * Expected: Register("28-123") == nullopt. Actual: optional(2) -> the sensor is registered under id 2.
 */
TEST(AuditBugs, F26_RegisterWithInvalidPhysicalIdReturnsErrorCodeAsSensorId) {
  srp::mw::temp::TempController c;
  auto id = c.Register("28-123");
  EXPECT_FALSE(id.has_value()) << "F-26: Register(\"28-123\") returned sensor id " << int(id.value_or(0))
                               << " (error code used as id, temp_controller.cpp:66-80)";
}
