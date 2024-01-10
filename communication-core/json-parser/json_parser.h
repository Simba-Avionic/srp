<<<<<<< HEAD
/**
 * @file json_parser.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_JSON_PARSER_JSON_PARSER_H_
#define COMMUNICATION_CORE_JSON_PARSER_JSON_PARSER_H_
#include <string>
#include <unordered_map>

#include "json-parser/Ijson_parser.h"
=======
#ifndef ROUTER_COMUNICATION_CORE_JSON_PARSER_JSON_PARSER_H_
#define ROUTER_COMUNICATION_CORE_JSON_PARSER_JSON_PARSER_H_
#include "communication-core/json-parser/Ijson_parser.h"
<<<<<<< HEAD
>>>>>>> 138e134 (fix structure and create DtcService without diag communication)
=======
>>>>>>> e74fb50a2127a25e8fd1f624ba4823088e10a74c

namespace simba {
namespace database {
namespace json {
class Json_parser : public Ijson_parser {
 private:
  simba::database::objects::AppElement ParseJson(
      const nlohmann::json& data) override;
  std::unordered_map<std::string, std::string> parsePubMethods(
      const nlohmann::json& json_data);
  std::unordered_map<std::string, simba::database::objects::ReqEventElement>
  Json_parser::parseReqEvents(const nlohmann::json& json_data);
  std::unordered_map<std::string, simba::database::objects::EventElement>
  Json_parser::parsePubEvents(const nlohmann::json& json_data);
  std::unordered_map<std::string, simba::database::objects::MethodElement>
  parseDb(const nlohmann::json& data);
  std::unordered_map<std::string, std::string> parseConfig(
      const nlohmann::json& json_data);

 public:
  Json_parser() = default;
  ~Json_parser() = default;
  simba::database::objects::AppElement LoadJson(
      const std::string& path) override;
};
}  // namespace json
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_JSON_PARSER_JSON_PARSER_H_
