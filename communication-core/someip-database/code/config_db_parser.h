/**
 * @file config_db_parser.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-04
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_CONFIG_DB_PARSER_H_
#define COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_CONFIG_DB_PARSER_H_

#include <iostream>
#include <memory>
#include <vector>

#include "communication-core/someip-database/code/config_db.h"
#include "communication-core/someip-database/code/endpoint.h"
#include "core/common/error_code.h"
#include "nlohmann/json.hpp"
namespace simba {
namespace com {
namespace someip {
namespace json {

class ConfigDbParser {
 public:
  static simba::core::ErrorCode ParseJsonObject(
      std::shared_ptr<objects::ConfigDb> db,
      const nlohmann::json& obj) noexcept {
    auto res = ParseReqMethods(db, obj);
    res = ParseProvideMethods(db, obj);
    res = ParsePubEvent(db, obj);
    res = ParseReqEvent(db, obj);

    return res;
  }

 private:
  static simba::core::ErrorCode ParseProvideMethods(
      std::shared_ptr<objects::ConfigDb> db,
      const nlohmann::json& obj) noexcept {
    if (!obj.contains("pub_methods")) {
      return simba::core::ErrorCode::kError;
    }
    auto list = obj["pub_methods"];
    for (auto& [key, val] : list.items()) {
      db->InsertObject(key, ParseObj(val));
    }
    return simba::core::ErrorCode::kOk;
  }

  static simba::core::ErrorCode ParseReqMethods(
      std::shared_ptr<objects::ConfigDb> db,
      const nlohmann::json& obj) noexcept {
    if (!obj.contains("req_methods")) {
      return simba::core::ErrorCode::kError;
    }
    auto list = obj["req_methods"];
    for (auto& [key, val] : list.items()) {
      db->InsertObject(key, ParseObj(val));
    }
    return simba::core::ErrorCode::kOk;
  }

  static simba::core::ErrorCode ParseReqEvent(
      std::shared_ptr<objects::ConfigDb> db,
      const nlohmann::json& obj) noexcept {
    if (!obj.contains("req_events")) {
      return simba::core::ErrorCode::kError;
    }
    auto list = obj["req_events"];
    for (auto& [key, val] : list.items()) {
      db->InsertObject(key, ParseObj(val));
    }
    return simba::core::ErrorCode::kOk;
  }
  static simba::core::ErrorCode ParsePubEvent(
      std::shared_ptr<objects::ConfigDb> db,
      const nlohmann::json& obj) noexcept {
    if (!obj.contains("pub_event")) {
      return simba::core::ErrorCode::kError;
    }
    auto list = obj["pub_event"];
    for (auto& [key, val] : list.items()) {
      db->InsertObject(key, ParseObj(val));
    }
    return simba::core::ErrorCode::kOk;
  }
  static objects::Endpoint ParseObj(const nlohmann::json& obj) noexcept {
    uint16_t method_id = 0;
    uint16_t service_id = 0;
    if (obj.contains("method_id")) {
      method_id = obj["method_id"];
    }
    if (obj.contains("event_id")) {
      method_id = obj["event_id"];
    }
    if (obj.contains("service_id")) {
      service_id = obj["service_id"];
    }
    if (obj.contains("access_list")) {
      auto list = obj["access_list"];
      std::vector<uint16_t> access_list{};
      for (auto& [key, val] : list.items()) {
        const uint16_t id = val;
        access_list.push_back(id);
      }
      return objects::Endpoint{service_id, method_id, access_list};
    } else {
      return objects::Endpoint{service_id, method_id};
    }
  }
};

}  // namespace json
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_CONFIG_DB_PARSER_H_
