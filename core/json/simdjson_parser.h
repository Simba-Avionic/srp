/**
 * @file simdjson_parser.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CORE_JSON_SIMDJSON_PARSER_H_
#define CORE_JSON_SIMDJSON_PARSER_H_

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include "simdjson.h"

namespace srp {
namespace core {
namespace json {

class JsonParser {
 private:
  std::shared_ptr<simdjson::dom::parser> parser_;
  simdjson::dom::element obj_;
  simdjson::error_code error_ = simdjson::SUCCESS;

  JsonParser(std::shared_ptr<simdjson::dom::parser> p, simdjson::dom::element e);

 public:
  bool IsValid() const noexcept { return error_ == simdjson::SUCCESS; }
  static std::optional<JsonParser> Parser(const std::string& path) noexcept;

  explicit JsonParser(const std::string& data);
  explicit JsonParser(simdjson::dom::element element);

  std::optional<JsonParser> GetObject(const std::string& name) const;
  simdjson::dom::element GetObject() const;

  std::optional<std::string> GetString(const std::string& name) const noexcept;

  template <typename T>
  std::optional<std::vector<T>> GetArray(const std::string& name) const {
    simdjson::dom::array arr;
    auto error = obj_[name].get<simdjson::dom::array>().get(arr);
    if (error) return std::nullopt;

    std::vector<T> res;
    for (simdjson::dom::element child : arr) {
        T value;
        if (child.get<T>().get(value) == simdjson::SUCCESS) {
            res.push_back(value);
        }
    }
    return res;
  }

  template <typename T>
  std::optional<T> GetNumber(const std::string& name) const noexcept {
    T value;
    auto error = obj_[name].get<T>().get(value);
    if (error) {
        return std::nullopt;
    }
    return value;
  }

  ~JsonParser() = default;
};

}  // namespace json
}  // namespace core
}  // namespace srp

#endif  // CORE_JSON_SIMDJSON_PARSER_H_
