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
  static void LoadJson(Database& db,                       // NOLINT
                       const std::string& path_to_json) {  // NOLINT
    std::ifstream f(path_to_json);
    if (!f.is_open()) {
      return;
    }
    nlohmann::json data = nlohmann::json::parse(f);
    ParseReqMethods(db, data);
    ParsePubMethods(db, data);
    ParseDb(db, data);
    ParsePubEvent(db, data);
    ParseReqEvent(db, data);
  }
  template <class Database>
  static void ParseReqMethods(Database& db,                  // NOLINT
                              const nlohmann::json& data) {  // NOLINT
    auto reqs_methods = data["req_methods"];
    for (auto& [key, val] : reqs_methods.items()) {
      db.AddReqMethodsElement(
          key, db.CreatReqMethodElement(val["method_id"], val["service_id"]));
    }
  }
  template <class Database>
  static void ParsePubMethods(Database& db,                  // NOLINT
                              const nlohmann::json& data) {  // NOLINT
    auto reqs_methods = data["pub_methods"];
    for (auto& [key, val] : reqs_methods.items()) {
      db.AddPubMethodElement(val["name"],
                             static_cast<uint16_t>(val["method_id"]));
    }
  }
  template <class Database>
  static void ParseDb(Database& db, const nlohmann::json& data) {  // NOLINT
    auto reqs_methods = data["db"];
    for (auto& [key, val] : reqs_methods.items()) {
      db.AddDbElement(
          static_cast<uint16_t>(std::stoi(key)),
          db.CreatDbElement((val["ip"].size() == 0) ? val["ipc"] : val["ip"],
                            static_cast<uint16_t>(val["port"])));
    }
  }
  template <class Database>
  static void ParsePubEvent(Database& db,                  // NOLINT
                            const nlohmann::json& data) {  // NOLINT
    auto reqs_methods = data["pub_events"];
    for (auto& [key, val] : reqs_methods.items()) {
      std::cout << "key: " << key << ", value:" << val << '\n';
    }
  }
  template <class Database>
  static void ParseReqEvent(Database& db,                  // NOLINT
                            const nlohmann::json& data) {  // NOLINT
    auto reqs_methods = data["req_events"];
    for (auto& [key, val] : reqs_methods.items()) {
      // std::cout << "key: " << key << ", value:" << val << '\n';

      db.AddReqMethodsElement(
          val["name"],
          db.CreatReqMethodElement(val["event_id"], val["service_id"]));
    }
  }
  DatabaseJsonParser(/* args */);
  ~DatabaseJsonParser();
};
}  // namespace json
}  // namespace database
}  // namespace simba

#endif  // COMMUNICATION_CORE_JSON_PARSER_DATABASE_JSON_PARSER_H_
