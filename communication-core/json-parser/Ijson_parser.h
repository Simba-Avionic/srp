/**
 * @file Ijson_parser.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_JSON_PARSER_IJSON_PARSER_H_
#define COMMUNICATION_CORE_JSON_PARSER_IJSON_PARSER_H_
#include <fstream>
#include <string>

#include "communication-core/database/app_element.h"
#include "nlohmann/json.hpp"
#include "unordered_map"
#include "communication-core/database/app_element.h"

namespace simba {
namespace database {
namespace json {
class Ijson_parser {
 private:
  virtual simba::database::objects::AppElement ParseJson(
      const nlohmann::json& data) = 0;

 public:
  virtual simba::database::objects::AppElement LoadJson(
      const std::string& path) = 0;
};
}  // namespace json
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_JSON_PARSER_IJSON_PARSER_H_
