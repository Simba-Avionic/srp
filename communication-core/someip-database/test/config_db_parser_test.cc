/**
 * @file config_db_parser_test.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "communication-core/someip-database/code/config_db_parser.h"

#include <gtest/gtest.h>

#include <fstream>
#include <memory>

#include "communication-core/someip-database/code/config_db.h"
#include "core/common/error_code.h"
#include "nlohmann/json.hpp"

TEST(DataBaseParser, Parsing) {
  std::ifstream f(
      "communication-core/someip-database/test/resource/app_someip1.json");
  auto db = std::make_shared<simba::com::someip::objects::ConfigDb>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            simba::com::someip::json::ConfigDbParser::ParseJsonObject(db, obj));
}

TEST(DataBaseParser, ParsingAndFindReqMethod) {
  std::ifstream f(
      "communication-core/someip-database/test/resource/app_someip1.json");
  auto db = std::make_shared<simba::com::someip::objects::ConfigDb>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            simba::com::someip::json::ConfigDbParser::ParseJsonObject(db, obj));
  EXPECT_TRUE(db->FindObject("ExampleApp/exampleMethod").has_value());
  const auto r = db->FindObject("ExampleApp/exampleMethod");
  EXPECT_EQ(10, r.value().GetEndpointId());
  EXPECT_EQ(15, r.value().GetServiceId());
}

TEST(DataBaseParser, ParsingAndFindPubMethod) {
  std::ifstream f(
      "communication-core/someip-database/test/resource/app_someip2.json");
  auto db = std::make_shared<simba::com::someip::objects::ConfigDb>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            simba::com::someip::json::ConfigDbParser::ParseJsonObject(db, obj));
  EXPECT_TRUE(db->FindObject("ExampleApp/exampleMethod").has_value());
  EXPECT_TRUE(db->FindObject("ExampleApp/exampleMethod2").has_value());
  const auto r = db->FindObject("ExampleApp/exampleMethod");
  EXPECT_EQ(10, r.value().GetEndpointId());
  EXPECT_EQ(0, r.value().GetServiceId());
  EXPECT_TRUE(r.value().CanPass(15));
  EXPECT_FALSE(r.value().CanPass(150));
}

TEST(DataBaseParser, ParsingAndFindReqEvent) {
  std::ifstream f(
      "communication-core/someip-database/test/resource/app_someip1.json");
  auto db = std::make_shared<simba::com::someip::objects::ConfigDb>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            simba::com::someip::json::ConfigDbParser::ParseJsonObject(db, obj));
  EXPECT_TRUE(db->FindObject("ExampleApp/exampleEvent").has_value());
  const auto r = db->FindObject("ExampleApp/exampleEvent");
  EXPECT_EQ(32769, r.value().GetEndpointId());
  EXPECT_EQ(15, r.value().GetServiceId());
}
TEST(DataBaseParser, ParsingAndFindPubEvent) {
  std::ifstream f(
      "communication-core/someip-database/test/resource/app_someip2.json");
  auto db = std::make_shared<simba::com::someip::objects::ConfigDb>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            simba::com::someip::json::ConfigDbParser::ParseJsonObject(db, obj));
  EXPECT_TRUE(db->FindObject("ExampleApp/exampleEvent").has_value());
  const auto r = db->FindObject("ExampleApp/exampleEvent");
  EXPECT_EQ(32769, r.value().GetEndpointId());
  EXPECT_EQ(0, r.value().GetServiceId());
}
