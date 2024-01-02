/**
 * @file database_json_parser.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define parser for database
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_JSON_PARSER_DATABASE_JSON_PARSER_H_
#define COMMUNICATION_CORE_JSON_PARSER_DATABASE_JSON_PARSER_H_
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
// using nlojson = nlohmann::json;
namespace simba {
namespace database {
namespace json {
class DatabaseJsonParser {
 private:
  /* data */
 public:
  template <class Database>
  static void LoadJson(Database& db,  // NOLINT
                       const std::string& path_to_json) {  // NOLINT
    std::ifstream f(path_to_json);
    if (!f.is_open()) {
      return;
    }
    nlohmann::json data = nlohmann::json::parse(f);
    nlohmann::json remote_lists = data.at("remote");
    if (!remote_lists.is_array()) {
      return;
    }
    for (auto obj : remote_lists) {
      try {
        const uint16_t service_id = obj["id"];
        const std::string ip = obj["ip"];
        const uint16_t port = obj["port"];
        const auto item = db.CreatDatabaseElement(ip, port);
        db.AddService(service_id, item);
      } catch (...) {
        continue;
      }
    }
  }

  DatabaseJsonParser(/* args */);
  ~DatabaseJsonParser();
};
}  // namespace json
}  // namespace database
}  // namespace simba

#endif  // COMMUNICATION_CORE_JSON_PARSER_DATABASE_JSON_PARSER_H_
