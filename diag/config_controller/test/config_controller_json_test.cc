/**
 * @file config_controller_json_test.cc
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "diag/config_controller/config_controller_json.h"

#include <gtest/gtest.h>

#include <fstream>
#include <memory>

#include "core/common/error_code.h"
#include "diag/config_controller/config_controller.h"
#include "nlohmann/json.hpp"

TEST(CONFIGJSONPARSER, Parsing) {
  std::ifstream f("diag/config_controller/test/resource/app_diag.json");
  auto db = std::make_shared<simba::diag::config::ConfigController>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(
      simba::core::ErrorCode::kOk,
      simba::diag::config::json::ConfigControllerJson::ParseConfig(obj, db));
  auto res = db->FindProvideObject("EC_DiagOtaApp/vin");
  EXPECT_TRUE(res.has_value());
  EXPECT_EQ("2232608", res.value());
  res = db->FindRequireObject("EC_DiagOtaApp/r_start");
  EXPECT_TRUE(res.has_value());
  EXPECT_EQ("3215648", res.value());
}
TEST(CONFIGJSONPARSER, ParsingError) {
  {
    std::ifstream f("diag/config_controller/test/resource/app_diag_f1.json");
    auto db = std::make_shared<simba::diag::config::ConfigController>();
    auto obj = nlohmann::json::parse(f);
    EXPECT_EQ(
        simba::core::ErrorCode::kError,
        simba::diag::config::json::ConfigControllerJson::ParseConfig(obj, db));
  }
  {
    std::ifstream f("diag/config_controller/test/resource/app_diag_f2.json");
    auto db = std::make_shared<simba::diag::config::ConfigController>();
    auto obj = nlohmann::json::parse(f);
    EXPECT_EQ(
        simba::core::ErrorCode::kError,
        simba::diag::config::json::ConfigControllerJson::ParseConfig(obj, db));
  }
}