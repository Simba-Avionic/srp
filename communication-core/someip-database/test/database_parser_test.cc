/**
 * @file database_parser_test.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "communication-core/someip-database/code/database_parser.h"

#include <gtest/gtest.h>

#include <fstream>
#include <memory>

#include "communication-core/someip-database/code/database.h"
#include "core/common/error_code.h"
#include "nlohmann/json.hpp"

TEST(DataBaseParser, Parsing) {
  std::ifstream f(
      "communication-core/someip-database/test/resource/app_someip1.json");
  auto db = std::make_shared<simba::com::someip::objects::DataBase>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            simba::com::someip::json::DataBaseParser::ParseJsonObject(db, obj));
}

TEST(DataBaseParser, GiveCorrectFileWhileFindServiceReturnPositive) {
  std::ifstream f(
      "communication-core/someip-database/test/resource/app_someip1.json");
  auto db = std::make_shared<simba::com::someip::objects::DataBase>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            simba::com::someip::json::DataBaseParser::ParseJsonObject(db, obj));
  EXPECT_TRUE(db->FindService(15).has_value());
  EXPECT_EQ(0, db->FindService(15).value().GetPort());
  EXPECT_EQ("SIMBA.SOMEIP.15", db->FindService(15).value().GetIp());
}

TEST(DataBaseParser, GiveCorrectFileWhileFindServiceReturnNegative) {
  std::ifstream f(
      "communication-core/someip-database/test/resource/app_someip1.json");
  auto db = std::make_shared<simba::com::someip::objects::DataBase>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            simba::com::someip::json::DataBaseParser::ParseJsonObject(db, obj));
  EXPECT_FALSE(db->FindService(30).has_value());
}

TEST(DataBaseParser, GiveCorrectFileWhileFindEventReturnPositive) {
  std::ifstream f(
      "communication-core/someip-database/test/resource/app_someip2.json");
  auto db = std::make_shared<simba::com::someip::objects::DataBase>();
  auto obj = nlohmann::json::parse(f);
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            simba::com::someip::json::DataBaseParser::ParseJsonObject(db, obj));
  EXPECT_TRUE(db->FindEventClient(32769).has_value());
}
