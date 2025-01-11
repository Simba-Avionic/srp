/**
 * @file json_parser.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/em/code/services/em/json_parser.h"

#include "ara/log/log.h"

namespace srp {
namespace em {
namespace service {
namespace json {
template <>
uint8_t JsonParser::GetDataFromJson<std::string>(
    std::string& desc, const core::json::JsonParser& json,  // NOLINT
    const std::string& tag, const std::string& path) {
  const auto temp_val = json.GetString(tag);
  if (temp_val.has_value()) {
    desc = temp_val.value();
    return 0;
  } else {
    ara::log::LogError() << "Application from: " << path
                         << ", don't have: " << tag;
    return 1;
  }
}

template <>
uint8_t JsonParser::GetDataFromJson<uint16_t>(
    uint16_t& desc, const core::json::JsonParser& json,
    const std::string& tag,  // NOLINT
    const std::string& path) {
  const auto temp_val = json.GetNumber<uint16_t>(tag);
  if (temp_val.has_value()) {
    desc = temp_val.value();
    return 0;
  } else {
    ara::log::LogError() << "Application from: " << path
                         << ", don't have: " << tag;
    return 1;
  }
}

template <>
uint8_t JsonParser::GetDataFromJson<std::vector<std::string>>(
    std::vector<std::string>& desc,  // NOLINT
    const core::json::JsonParser& json, const std::string& tag,
    const std::string& path) {
  const auto temp_val = json.GetArray<std::string>(tag);
  if (temp_val.has_value()) {
    const auto t = temp_val.value();
    desc.insert(desc.end(), t.begin(), t.end());
    return 0;
  } else {
    ara::log::LogError() << "Application from: " << path
                         << ", don't have: " << tag;
    return 1;
  }
}

std::optional<data::AppConfig> JsonParser::GetAppConfig(
    const std::string& path) noexcept {
  auto obj = core::json::JsonParser::Parser(path).value();
  std::string bin_path{""};
  std::string parm{""};
  uint8_t error_count{0};
  uint16_t app_id{0};
  std::string app_name{""};
  std::vector<std::string> fg_list{};
  error_count +=
      JsonParser::GetDataFromJson<std::string>(app_name, obj, "app_name", path);
  error_count +=
      JsonParser::GetDataFromJson<uint16_t>(app_id, obj, "app_id", path);
  error_count +=
      JsonParser::GetDataFromJson<std::string>(bin_path, obj, "bin_path", path);
  error_count +=
      JsonParser::GetDataFromJson<std::string>(parm, obj, "parms", path);
  error_count += JsonParser::GetDataFromJson<std::vector<std::string>>(
      fg_list, obj, "fg_list", path);

  if (error_count != 0) {
    return {};
  } else {
    return std::optional{
        data::AppConfig{bin_path, parm, app_id, app_name, fg_list}};
  }
}

}  // namespace json
}  // namespace service
}  // namespace em
}  // namespace srp
