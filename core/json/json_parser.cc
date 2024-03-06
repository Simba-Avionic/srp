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
    return std::optional<JsonParser>{};
  }
  return std::optional<JsonParser>{JsonParser{f}};
}
JsonParser::JsonParser(std::ifstream& f) {
    this->obj = nlohmann::json::parse(f);
}
nlohmann::json JsonParser::GetObject() const { return this->obj; }
std::optional<std::string> JsonParser::GetString(
    const std::string& name) const noexcept {
  try {
    if (obj.contains(name)) {
      if (obj.at(name).is_string()) {
        const std::string res = obj.at(name);
        return std::optional{res};
      }
      return std::optional<std::string>{};
    } else {
      return std::optional<std::string>{};
    }
  } catch (std::exception&) {
    return std::optional<std::string>{};
  }
}
JsonParser::~JsonParser() {}

}  // namespace json
}  // namespace core
}  // namespace simba
