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
  static core::ErrorCode ParseJsonObject(std::shared_ptr<objects::ConfigDb> db,
                                         const nlohmann::json& obj) noexcept {
    auto res = ParseReqMethods(db, obj);
    if (res == core::ErrorCode::kOk) {
      res = ParseProvideMethods(db, obj);
    }
    if (res == core::ErrorCode::kOk) {
      res = ParsePubEvent(db, obj);
    }
    if (res == core::ErrorCode::kOk) {
      res = ParseReqEvent(db, obj);
    }

    return res;
  }

  static core::ErrorCode ParseProvideMethods(
      std::shared_ptr<objects::ConfigDb> db,
      const nlohmann::json& obj) noexcept {
    auto list = obj["pub_methods"];
    for (auto& [key, val] : list.items()) {
      db->InsertObject(key, ParseObj(val));
    }
    return core::ErrorCode::kOk;
  }

  static core::ErrorCode ParseReqMethods(std::shared_ptr<objects::ConfigDb> db,
                                         const nlohmann::json& obj) noexcept {
    auto list = obj["req_methods"];
    for (auto& [key, val] : list.items()) {
      db->InsertObject(key, ParseObj(val));
    }
    return core::ErrorCode::kOk;
  }

  static core::ErrorCode ParseReqEvent(std::shared_ptr<objects::ConfigDb> db,
                                       const nlohmann::json& obj) noexcept {
    auto list = obj["req_events"];
    for (auto& [key, val] : list.items()) {
      db->InsertObject(key, ParseObj(val));
    }
    return core::ErrorCode::kOk;
  }
  static core::ErrorCode ParsePubEvent(std::shared_ptr<objects::ConfigDb> db,
                                       const nlohmann::json& obj) noexcept {
    auto list = obj["pub_event"];
    for (auto& [key, val] : list.items()) {
      db->InsertObject(key, ParseObj(val));
    }
    return core::ErrorCode::kOk;
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
    return objects::Endpoint{service_id, method_id};
  }
};

}  // namespace json
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_CONFIG_DB_PARSER_H_
