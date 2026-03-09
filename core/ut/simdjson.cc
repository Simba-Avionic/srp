/**
 * @file simdjson.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "core/json/simdjson_parser.h"
#include <gtest/gtest.h>
#include <fstream>

namespace srp {
namespace core {
namespace json {

class JsonParserTest : public ::testing::Test {
 protected:
  // Przykładowy JSON do testów
  const std::string valid_json = R"({
    "app": "simba",
    "version": 1,
    "pi": 3.14,
    "active": true,
    "settings": {
      "theme": "dark",
      "notifications": true
    },
    "tags": ["core", "json", "fast"]
  })";

  const std::string invalid_json = R"({ "unclosed": "brace" )";
};

// Test sprawdzający parsowanie poprawnego stringa
TEST_F(JsonParserTest, ParseValidString) {
  JsonParser parser(valid_json);
  auto app_name = parser.GetString("app");

  ASSERT_TRUE(app_name.has_value());
  EXPECT_EQ(app_name.value(), "simba");
}

// Test sprawdzający zachowanie przy nieistniejącym kluczu
TEST_F(JsonParserTest, GetNonExistentKey) {
  JsonParser parser(valid_json);
  auto missing = parser.GetString("non_existent");

  EXPECT_FALSE(missing.has_value());
}

// Test sprawdzający pobieranie liczb (int i double)
TEST_F(JsonParserTest, GetNumbers) {
  JsonParser parser(valid_json);

  auto version = parser.GetNumber<int64_t>("version");
  auto pi = parser.GetNumber<double>("pi");

  ASSERT_TRUE(version.has_value());
  EXPECT_EQ(version.value(), 1);

  ASSERT_TRUE(pi.has_value());
  EXPECT_DOUBLE_EQ(pi.value(), 3.14);
}

// Test sprawdzający pobieranie zagnieżdżonego obiektu
TEST_F(JsonParserTest, GetNestedObject) {
  JsonParser parser(valid_json);
  auto settings = parser.GetObject("settings");

  ASSERT_TRUE(settings.has_value());

  auto theme = settings->GetString("theme");
  ASSERT_TRUE(theme.has_value());
  EXPECT_EQ(theme.value(), "dark");
}

// Test sprawdzający pobieranie tablicy
TEST_F(JsonParserTest, GetArray) {
  JsonParser parser(valid_json);
  auto tags = parser.GetArray<std::string_view>("tags");

  ASSERT_TRUE(tags.has_value());
  EXPECT_EQ(tags->size(), 3);
  EXPECT_EQ(tags->at(0), "core");
}

TEST_F(JsonParserTest, ParseInvalidJson) {
  JsonParser parser(invalid_json);
  EXPECT_FALSE(parser.IsValid());
  EXPECT_FALSE(parser.GetString("app").has_value());
}

// Test statycznej metody ładującej z pliku
TEST_F(JsonParserTest, LoadFromFile) {
  const std::string filename = "test_temp.json";
  std::ofstream out(filename);
  out << valid_json;
  out.close();
  auto parser_opt = JsonParser::Parser(filename);

  ASSERT_TRUE(parser_opt.has_value());
  EXPECT_EQ(parser_opt->GetString("app").value_or(""), "simba");
  std::remove(filename.c_str());
}

}  // namespace json
}  // namespace core
}  // namespace srp
