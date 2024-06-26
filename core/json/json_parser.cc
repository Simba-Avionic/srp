/**
 * @file json_parser.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "core/json/json_parser.h"

#include <fstream>
namespace simba {
namespace core {
namespace json {

std::optional<JsonParser> JsonParser::Parser(const std::string& path) noexcept {
  std::ifstream f(path);
  if (!f.is_open()) {
    return {};
  }
  return std::optional<JsonParser>{JsonParser{f}};
}
std::optional<JsonParser> JsonParser::Parser(nlohmann::json obj) noexcept {
  return std::optional<JsonParser>{JsonParser{obj}};
}
JsonParser::JsonParser(std::ifstream& f) {
  this->obj = nlohmann::json::parse(f);
}
JsonParser::JsonParser(nlohmann::json json) {
  this->obj = json;
}
nlohmann::json JsonParser::GetObject() const { return this->obj; }

std::optional<nlohmann::json> JsonParser::GetArray(const std::string& name) {
  if (!obj.contains(name)) {
    return {};
  }
  if (!obj.at(name).is_array()) {
    return {};
  }
  return obj.at(name);
}

std::optional<std::string> JsonParser::GetString(
    const std::string& name) const noexcept {
  try {
    if (obj.contains(name)) {
      if (obj.at(name).is_string()) {
        const std::string res = obj.at(name);
        return std::optional{res};
      }
      return {};
    } else {
      return {};
    }
  } catch (std::exception&) {
    return {};
  }
}
JsonParser::~JsonParser() {}

}  // namespace json
}  // namespace core
}  // namespace simba
