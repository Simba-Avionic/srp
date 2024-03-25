/**
 * @file database_parser.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-25
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_DATABASE_PARSER_H_
#define COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_DATABASE_PARSER_H_
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "communication-core/someip-database/code/database.h"
#include "communication-core/someip-database/code/interface.h"
#include "core/common/error_code.h"
#include "nlohmann/json.hpp"

namespace simba {
namespace com {
namespace someip {
namespace json {
class DataBaseParser {
 public:
  static simba::core::ErrorCode ParseJsonObject(std::shared_ptr<objects::DataBase> db,
                                         const nlohmann::json& obj) noexcept {
    simba::core::ErrorCode flag = simba::core::ErrorCode::kOk;
    if (DataBaseParser::ValidateDb(obj) != simba::core::ErrorCode::kOk) {
      return simba::core::ErrorCode::kError;
    }
    auto list = obj.at("db");
    for (auto& [key, val] : list.items()) {
      if (DataBaseParser::ValidateItem(val) == simba::core::ErrorCode::kOk) {
        std::string ip = val.at("ip");
        uint16_t port = static_cast<uint16_t>(val.at("port"));
        if (db->InsertService(static_cast<uint16_t>(std::stoi(key)),
                              objects::Interface{ip, port}) !=
            simba::core::ErrorCode::kOk) {
          flag = simba::core::ErrorCode::kError;
        }
      } else {
        flag = simba::core::ErrorCode::kError;
      }
    }
    if (flag == simba::core::ErrorCode::kOk) {
      flag = ParseEventClients(db, obj);
    }
    if (flag == simba::core::ErrorCode::kOk) {
      flag = ParseConfig(std::move(db), obj);
    }
    return flag;
  }

 private:
  static simba::core::ErrorCode ValidateDb(const nlohmann::json& obj) noexcept {
    if (!obj.contains("interface")) {
      return simba::core::ErrorCode::kError;
    }
    if (!obj.contains("service_id")) {
      return simba::core::ErrorCode::kError;
    }
    if (!obj.contains("db")) {
      return simba::core::ErrorCode::kError;
    }
    return simba::core::ErrorCode::kOk;
  }
  static simba::core::ErrorCode ValidateItem(const nlohmann::json& obj) noexcept {
    if (!obj.contains("ip")) {
      return simba::core::ErrorCode::kError;
    }
    if (!obj.contains("port")) {
      return simba::core::ErrorCode::kError;
    }
    if (!obj.at("ip").is_string()) {
      return simba::core::ErrorCode::kError;
    }
    if (!obj.at("port").is_number_unsigned()) {
      return simba::core::ErrorCode::kError;
    }
    return simba::core::ErrorCode::kOk;
  }
  static simba::core::ErrorCode ParseEventClients(
      std::shared_ptr<objects::DataBase> db, const nlohmann::json& obj) {
    auto list = obj["pub_event"];
    for (auto& [key, val] : list.items()) {
      const uint16_t event_id = val["event_id"];
      const std::vector<uint16_t> list2 = val["subscribers"];
      for (const auto& val : list2) {
        db->InstertEvent(event_id, val);
      }
    }
    return simba::core::ErrorCode::kOk;
  }
  static simba::core::ErrorCode ParseConfig(std::shared_ptr<objects::DataBase> db,
                                     const nlohmann::json& obj) {
    const uint16_t id = obj.at("service_id");
    db->SetServiceId(id);
    auto list = obj.at("interface");
    for (auto& obj : list.items()) {
      const std::string ip = obj.value()["ip"];
      const uint16_t port = obj.value()["port"];
      db->InsertInterface(ip, port);
    }
    return simba::core::ErrorCode::kOk;
  }
};

}  // namespace json
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_DATABASE_PARSER_H_
