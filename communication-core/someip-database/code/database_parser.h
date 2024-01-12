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
  static core::ErrorCode ParseJsonObject(std::shared_ptr<objects::DataBase> db,
                                         const nlohmann::json& obj) noexcept {
    core::ErrorCode flag = core::ErrorCode::kOk;
    if (DataBaseParser::ValidateDb(obj) != core::ErrorCode::kOk) {
      return core::ErrorCode::kError;
    }
    auto list = obj.at("db");
    for (auto& [key, val] : list.items()) {
      if (DataBaseParser::ValidateItem(val) == core::ErrorCode::kOk) {
        std::string ip = val.at("ip");
        uint16_t port = static_cast<uint16_t>(val.at("port"));
        if (db->InsertService(static_cast<uint16_t>(std::stoi(key)),
                              objects::Interface{ip, port}) !=
            core::ErrorCode::kOk) {
          flag = core::ErrorCode::kError;
        }
      } else {
        flag = core::ErrorCode::kError;
      }
    }
    if (flag == core::ErrorCode::kOk) {
      flag = ParseEventClients(std::move(db), obj);
    }
    return flag;
  }

 private:
  static core::ErrorCode ValidateDb(const nlohmann::json& obj) noexcept {
    if (!obj.contains("db")) {
      return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
  }
  static core::ErrorCode ValidateItem(const nlohmann::json& obj) noexcept {
    if (!obj.contains("ip")) {
      return core::ErrorCode::kError;
    }
    if (!obj.contains("port")) {
      return core::ErrorCode::kError;
    }
    if (!obj.at("ip").is_string()) {
      return core::ErrorCode::kError;
    }
    if (!obj.at("port").is_number_unsigned()) {
      return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
  }
  static core::ErrorCode ParseEventClients(
      std::shared_ptr<objects::DataBase> db, const nlohmann::json& obj) {
    auto list = obj["pub_event"];
    for (auto& [key, val] : list.items()) {
      const uint16_t event_id = val["event_id"];
      const std::vector<uint16_t> list2 = val["subscribers"];
      for (const auto& val : list2) {
        db->InstertEvent(event_id, val);
      }
    }
    return core::ErrorCode::kOk;
  }
};

}  // namespace json
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_DATABASE_PARSER_H_
