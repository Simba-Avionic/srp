/**
 * @file primer_config_test.cpp
 * @brief Testy parsowania konfiguracji zapalnika (F-06)
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <optional>
#include <string>

#include "gtest/gtest.h"
#include "apps/ec/primer_service/controller/primer_controller.hpp"
#include "core/json/json_parser.h"

namespace srp {
namespace primer {
namespace {

PrimerConfig Parse(const std::string& json) {
  return PrimerController::ParseConfig(core::json::JsonParser(json));
}

TEST(PrimerConfigTest, NoConfigGivesDefaults) {
  const auto cfg = PrimerController::ParseConfig(std::nullopt);
  ASSERT_EQ(cfg.pins.size(), 1u);
  EXPECT_EQ(cfg.pins[0], 13);
  EXPECT_EQ(cfg.active_time_ms, 2500);
}

TEST(PrimerConfigTest, ReadsPinsAndTime) {
  const auto cfg = Parse(R"({"primer":{"primer_ids":[5,7],"active_time":1200}})");
  ASSERT_EQ(cfg.pins.size(), 2u);
  EXPECT_EQ(cfg.pins[0], 5);
  EXPECT_EQ(cfg.pins[1], 7);
  EXPECT_EQ(cfg.active_time_ms, 1200);
}

TEST(PrimerConfigTest, MissingSectionGivesDefaults) {
  const auto cfg = Parse(R"({"other":{}})");
  ASSERT_EQ(cfg.pins.size(), 1u);
  EXPECT_EQ(cfg.pins[0], 13);
  EXPECT_EQ(cfg.active_time_ms, 2500);
}

TEST(PrimerConfigTest, InvalidValuesGiveDefaults) {
  for (const char* json : {
           R"({"primer":{"primer_ids":[],"active_time":0}})",
           R"({"primer":{"primer_ids":[300],"active_time":70000}})",
           R"({"primer":{"primer_ids":[-1],"active_time":-5}})",
           R"({"primer":{"primer_ids":"x","active_time":"y"}})"}) {
    const auto cfg = Parse(json);
    ASSERT_EQ(cfg.pins.size(), 1u) << json;
    EXPECT_EQ(cfg.pins[0], 13) << json;
    EXPECT_EQ(cfg.active_time_ms, 2500) << json;
  }
}

TEST(PrimerConfigTest, PartialConfigKeepsOtherDefault) {
  const auto cfg = Parse(R"({"primer":{"active_time":800}})");
  ASSERT_EQ(cfg.pins.size(), 1u);
  EXPECT_EQ(cfg.pins[0], 13);
  EXPECT_EQ(cfg.active_time_ms, 800);
}

}  // namespace
}  // namespace primer
}  // namespace srp
