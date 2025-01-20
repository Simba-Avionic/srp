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
#include <optional>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

namespace srp {
namespace core {
namespace json {
class JsonParser {
 private:
  nlohmann::json obj;

 public:
  static std::optional<JsonParser> Parser(const std::string& path) noexcept;
  explicit JsonParser(const std::string& data);
  explicit JsonParser(nlohmann::json json);
  static std::optional<JsonParser> Parser(nlohmann::json obj) noexcept;
  std::optional<JsonParser> GetObject(const std::string& name) const;
  nlohmann::json GetObject() const;
  std::optional<std::string> GetString(const std::string& name) const noexcept;
  // std::optional<nlohmann::json> GetArray(const std::string& name);
  template <typename T>
  std::optional<std::vector<T>> GetArray(const std::string& name) const {
    if (!obj.contains(name)) {
      return std::nullopt;
    }
    if (!obj.at(name).is_array()) {
      return std::nullopt;
    }
    std::vector<T> res;
    for (const T& value : obj.at(name)) {
      res.push_back(value);
    }
    return res;
  }
  template <typename T>
  std::optional<T> GetNumber(const std::string& name) const noexcept {
    try {
      if (obj.contains(name)) {
        if (obj.at(name).is_number()) {
          const T res{static_cast<T>(obj.at(name))};
          return res;
        }
        return std::nullopt;
      } else {
        return std::nullopt;
      }
    } catch (std::exception&) {
      return std::nullopt;
    }
  }
  ~JsonParser();
};

}  // namespace json
}  // namespace core
}  // namespace srp

#endif  // CORE_JSON_JSON_PARSER_H_
