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
namespace srp {
namespace core {
namespace json {

std::optional<JsonParser> JsonParser::Parser(const std::string& path) noexcept {
  std::ifstream f(path);
  if (!f.is_open()) {
    return std::nullopt;
  }
  std::stringstream buf;
  buf << f.rdbuf();
  f.close();
  std::string content = buf.str();
  if (!nlohmann::json::accept(content, true)) {
    return std::nullopt;
  }
  return std::optional<JsonParser>{JsonParser{content}};
}
std::optional<JsonParser> JsonParser::Parser(nlohmann::json obj) noexcept {
  return std::optional<JsonParser>{JsonParser{obj}};
}
JsonParser::JsonParser(const std::string& data) {
  this->obj = nlohmann::json::parse(data);
}
JsonParser::JsonParser(nlohmann::json json) {
  this->obj = json;
}
nlohmann::json JsonParser::GetObject() const { return this->obj; }

std::optional<JsonParser> JsonParser::GetObject(const std::string &name) const {
  if (!obj.contains(name)) {
    return std::nullopt;
  }
  if (!obj.at(name).is_object()) {
    return std::nullopt;
  }
  return JsonParser::Parser(obj.at(name));
}

// std::optional<nlohmann::json> JsonParser::GetArray(const std::string& name) {
//   if (!obj.contains(name)) {
//     return {};
//   }
//   if (!obj.at(name).is_array()) {
//     return {};
//   }
//   return obj.at(name);
// }

std::optional<std::string> JsonParser::GetString(
    const std::string& name) const noexcept {
  try {
    if (obj.contains(name)) {
      if (obj.at(name).is_string()) {
        const std::string res = obj.at(name);
        return std::optional{res};
      }
      return std::nullopt;
    } else {
      return std::nullopt;
    }
  } catch (std::exception&) {
    return std::nullopt;
  }
}
JsonParser::~JsonParser() {}

}  // namespace json
}  // namespace core
}  // namespace srp
