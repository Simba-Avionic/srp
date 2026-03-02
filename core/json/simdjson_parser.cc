/**
 * @file simdjson_parser.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "core/json/simdjson_parser.h"
 #include <memory>
#include <optional>
#include <string>

namespace srp {
namespace core {
namespace json {

std::optional<JsonParser> JsonParser::Parser(const std::string& path) noexcept {
  auto parser = std::make_shared<simdjson::dom::parser>();
  simdjson::dom::element doc;

  auto error = parser->load(path).get(doc);

  if (error) {
    return std::nullopt;
  }
  return JsonParser{parser, doc};
}

JsonParser::JsonParser(const std::string& data) {
  parser_ = std::make_shared<simdjson::dom::parser>();
  error_ = parser_->parse(data).get(obj_);
}

JsonParser::JsonParser(simdjson::dom::element element)
    : parser_(std::make_shared<simdjson::dom::parser>()), obj_(element) {}

JsonParser::JsonParser(std::shared_ptr<simdjson::dom::parser> p, simdjson::dom::element e)
    : parser_(p), obj_(e) {}

simdjson::dom::element JsonParser::GetObject() const {
    return obj_;
}

std::optional<JsonParser> JsonParser::GetObject(const std::string &name) const {
  simdjson::dom::element child;
  auto error = obj_.at_key(name).get(child);

  if (error) {
    return std::nullopt;
  }
  if (!child.is_object()) {
      return std::nullopt;
  }
  return JsonParser{this->parser_, child};
}

std::optional<std::string> JsonParser::GetString(const std::string& name) const noexcept {
  if (!IsValid()) return std::nullopt;

  std::string_view res;
  auto error = obj_.at_key(name).get<std::string_view>().get(res);
  if (error) return std::nullopt;

  return std::string(res);
}

}  // namespace json
}  // namespace core
}  // namespace srp
