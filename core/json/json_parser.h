/**
 * @file json_parser.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef CORE_JSON_JSON_PARSER_H_
#define CORE_JSON_JSON_PARSER_H_

#include <fstream>
#include <string>
#include <optional>

#include "nlohmann/json.hpp"

namespace simba {
namespace core {
namespace json {
class JsonParser {
 private:
  nlohmann::json obj;

 public:
  static std::optional<JsonParser> Parser(const std::string& path) noexcept;
  explicit JsonParser(std::ifstream& f);
  explicit JsonParser(nlohmann::json json);
  static std::optional<JsonParser> Parser(nlohmann::json obj) noexcept;
  nlohmann::json GetObject() const;
  std::optional<std::string> GetString(const std::string& name) const noexcept;
  std::optional<nlohmann::json> GetArray(const std::string& name);
  template <typename T>
  std::optional<T> GetNumber(const std::string& name) const noexcept {
    try {
      if (obj.contains(name)) {
        if (obj.at(name).is_number()) {
          const T res{static_cast<T>(obj.at(name))};
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
  ~JsonParser();
};

}  // namespace json
}  // namespace core
}  // namespace simba

#endif  // CORE_JSON_JSON_PARSER_H_
