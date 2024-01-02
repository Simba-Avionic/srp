#ifndef ROUTER_COMUNICATION_CORE_JSON_PARSER_JSON_PARSER_H_
#define ROUTER_COMUNICATION_CORE_JSON_PARSER_JSON_PARSER_H_
#include "json-parser/Ijson_parser.h"

namespace simba {
namespace database {
namespace json {
class Json_parser : public Ijson_parser {
    private:
     simba::database::objects::AppElement ParseJson(const nlohmann::json& data) override;
     std::unordered_map<std::string, std::string> parsePubMethods(const nlohmann::json& json_data);
     std::unordered_map<std::string, simba::database::objects::ReqEventElement> Json_parser::parseReqEvents(const nlohmann::json& json_data);
     std::unordered_map<std::string, simba::database::objects::EventElement> Json_parser::parsePubEvents(const nlohmann::json& json_data);
     std::unordered_map<std::string, simba::database::objects::MethodElement> parseDb(const nlohmann::json& data);
     std::unordered_map<std::string, std::string> parseConfig(const nlohmann::json& json_data);
    public:
     Json_parser() = default;
     ~Json_parser() = default;
     simba::database::objects::AppElement LoadJson(const std::string& path) override;
};
} //namespace simba
} //namespace database
} //namespace json
#endif ROUTER_COMUNICATION_CORE_JSON_PARSER_JSON_PARSER_H_