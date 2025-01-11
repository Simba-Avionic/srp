/**
 * @file json.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include <tuple>
#include <vector>

#include "core/json/json_parser.h"

class InitializeJsonParserTest: public testing::TestWithParam<std::tuple<nlohmann::json, bool>> {
 protected:
  nlohmann::json data;
  bool ans;
  void SetUp() override {
    data = std::get<0>(GetParam());
    ans = std::get<1>(GetParam());
  }
};

TEST_P(InitializeJsonParserTest, InitializeTest) {
    auto parser = srp::core::json::JsonParser::Parser(this->data);
    EXPECT_EQ(parser.has_value(), ans);
}

INSTANTIATE_TEST_SUITE_P(
    ErrorInitialize, InitializeJsonParserTest,
    testing::Values(
        std::make_tuple(nlohmann::json(R"({ "value": 0})"), true),
        std::make_tuple(nlohmann::json(R"({ "value"})"), true)
    )
);

class GetNumberTest: public testing::TestWithParam<std::tuple<std::string, int>> {
 protected:
  std::string data;
  int ans;
  void SetUp() override {
    data = std::get<0>(GetParam());
    ans = std::get<1>(GetParam());
  }
};

TEST_P(GetNumberTest, GetNumberTest) {
    auto parser = srp::core::json::JsonParser::Parser(nlohmann::json::parse(data));
    ASSERT_TRUE(parser.has_value());
    auto val = parser.value().GetNumber<int>("value");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), ans);
}

INSTANTIATE_TEST_SUITE_P(
    GetNum, GetNumberTest,
    testing::Values(
        std::make_tuple(nlohmann::json(R"({ "value": 0})"), 0),
        std::make_tuple(nlohmann::json(R"({ "value": 255})"), 255),
        std::make_tuple(nlohmann::json(R"({ "value": -12})"), -12),
        std::make_tuple(nlohmann::json(R"({ "value": 20202})"), 20202),
        std::make_tuple(nlohmann::json(R"({ "value": -555})"), -555),
        std::make_tuple(nlohmann::json(R"({ "value": -0})"), 0)
    )
);
// TODO dokończenie UT dla jsonParser  // NOLINT
