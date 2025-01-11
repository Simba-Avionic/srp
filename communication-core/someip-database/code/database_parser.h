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

namespace srp {
namespace com {
namespace someip {
namespace json {
class DataBaseParser {
 public:
  static srp::core::ErrorCode ParseJsonObject(std::shared_ptr<objects::DataBase> db,
                                         const nlohmann::json& obj) noexcept {
    srp::core::ErrorCode flag = srp::core::ErrorCode::kOk;
    if (DataBaseParser::ValidateDb(obj) != srp::core::ErrorCode::kOk) {
      return srp::core::ErrorCode::kError;
    }
    auto list = obj.at("db");
    for (auto& [key, val] : list.items()) {
      if (DataBaseParser::ValidateItem(val) == srp::core::ErrorCode::kOk) {
        std::string ip = val.at("ip");
        uint16_t port = static_cast<uint16_t>(val.at("port"));
        if (db->InsertService(static_cast<uint16_t>(std::stoi(key)),
                              objects::Interface{ip, port}) !=
            srp::core::ErrorCode::kOk) {
          flag = srp::core::ErrorCode::kError;
        }
      } else {
        flag = srp::core::ErrorCode::kError;
      }
    }
    if (flag == srp::core::ErrorCode::kOk) {
      flag = ParseEventClients(db, obj);
    }
    if (flag == srp::core::ErrorCode::kOk) {
      flag = ParseConfig(std::move(db), obj);
    }
    return flag;
  }

 private:
  static srp::core::ErrorCode ValidateDb(const nlohmann::json& obj) noexcept {
    if (!obj.contains("interface")) {
      return srp::core::ErrorCode::kError;
    }
    if (!obj.contains("service_id")) {
      return srp::core::ErrorCode::kError;
    }
    if (!obj.contains("db")) {
      return srp::core::ErrorCode::kError;
    }
    return srp::core::ErrorCode::kOk;
  }
  static srp::core::ErrorCode ValidateItem(const nlohmann::json& obj) noexcept {
    if (!obj.contains("ip")) {
      return srp::core::ErrorCode::kError;
    }
    if (!obj.contains("port")) {
      return srp::core::ErrorCode::kError;
    }
    if (!obj.at("ip").is_string()) {
      return srp::core::ErrorCode::kError;
    }
    if (!obj.at("port").is_number_unsigned()) {
      return srp::core::ErrorCode::kError;
    }
    return srp::core::ErrorCode::kOk;
  }
  static srp::core::ErrorCode ParseEventClients(
      std::shared_ptr<objects::DataBase> db, const nlohmann::json& obj) {
    auto list = obj["pub_event"];
    for (auto& [key, val] : list.items()) {
      const uint16_t event_id = val["event_id"];
      const std::vector<uint16_t> list2 = val["subscribers"];
      for (const auto& val : list2) {
        db->InstertEvent(event_id, val);
      }
    }
    return srp::core::ErrorCode::kOk;
  }
  static srp::core::ErrorCode ParseConfig(std::shared_ptr<objects::DataBase> db,
                                     const nlohmann::json& obj) {
    const uint16_t id = obj.at("service_id");
    db->SetServiceId(id);
    auto list = obj.at("interface");
    for (auto& obj : list.items()) {
      const std::string ip = obj.value()["ip"];
      const uint16_t port = obj.value()["port"];
      db->InsertInterface(ip, port);
    }
    return srp::core::ErrorCode::kOk;
  }
};

}  // namespace json
}  // namespace someip
}  // namespace com
}  // namespace srp
#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_DATABASE_PARSER_H_
