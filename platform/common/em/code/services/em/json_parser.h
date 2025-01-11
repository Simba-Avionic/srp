/**
 * @file json_parser.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_EM_CODE_SERVICES_EM_JSON_PARSER_H_
#define PLATFORM_COMMON_EM_CODE_SERVICES_EM_JSON_PARSER_H_

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "app_config.h"
#include "core/json/json_parser.h"

namespace srp {
namespace em {
namespace service {
namespace json {
class JsonParser {
 private:
  template <typename T>
  static uint8_t GetDataFromJson(T& desc,  // NOLINT
                                 const core::json::JsonParser& json,
                                 const std::string& tag,
                                 const std::string& path);

 public:
  static std::optional<data::AppConfig> GetAppConfig(
      const std::string& path) noexcept;
};
}  // namespace json
}  // namespace service
}  // namespace em
}  // namespace srp
#endif  // PLATFORM_COMMON_EM_CODE_SERVICES_EM_JSON_PARSER_H_
