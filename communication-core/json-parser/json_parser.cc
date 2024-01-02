#include "json_parser.h"
#include <fstream>

namespace simba {
namespace database {
namespace json {

simba::database::objects::AppElement Json_parser::LoadJson(const std::string& path) {
    std::ifstream file(path);
    if (file.is_open()) {
        nlohmann::json data;
        file >> data;
        file.close();

        simba::database::objects::AppElement result = ParseJson(data);
        return result;
    } else {
        // Handle file open error
    }
}

simba::database::objects::AppElement Json_parser::ParseJson(const nlohmann::json& data) {
    
    std::string name = data["name"];
    simba::database::objects::NetInterfaceElement buffer(data["service_id"], data["interface"]["ip"], data["interface"]["port"], data["interface"]["ipc"]);
    std::unordered_map<std::string, std::string> pub_methods = parsePubMethods(data);
    std::unordered_map<std::string, simba::database::objects::ReqEventElement> req_events = parseReqEvents(data);
    std::unordered_map<std::string, simba::database::objects::EventElement> pub_events = parsePubEvents(data);
    std::unordered_map<std::string, simba::database::objects::MethodElement> db = parseDb(data);
    std::unordered_map<std::string, std::string> conf = parseConfig(data);
    simba::database::objects::AppElement results(name, buffer, pub_methods, req_events, pub_events, db, conf);

    return results;
}

std::unordered_map<std::string, std::string> Json_parser::parsePubMethods(const nlohmann::json& json_data) {
    std::unordered_map<std::string, std::string> pub_methods;

    auto pub_methods_array = json_data["pub_methods"];

    for (const auto& method : pub_methods_array) {
        uint16_t method_id = method["method_id"];
        std::string name = method["name"];

        pub_methods[std::to_string(method_id)] = name;
    }

    return pub_methods;
}

std::unordered_map<std::string, simba::database::objects::ReqEventElement> Json_parser::parseReqEvents(const nlohmann::json& json_data) {
    std::unordered_map<std::string, simba::database::objects::ReqEventElement> req_events;

    auto req_events_array = json_data["req_events"];

    for (const auto& event : req_events_array) {
        u_int16_t event_id = event["event_id"];
        u_int16_t service_id = event["service_id"];
        std::string name = event["name"];

        simba::database::objects::ReqEventElement event_element(event_id, service_id);

        req_events.emplace(name, event_element);
    }

    return req_events;
}


std::unordered_map<std::string, simba::database::objects::EventElement> Json_parser::parsePubEvents(const nlohmann::json& json_data) {
    std::unordered_map<std::string, simba::database::objects::EventElement> pub_events;

    for (auto it = json_data["pub_events"].begin(); it != json_data["pub_events"].end(); ++it) {
        const std::string& event_key = it.key();
        const nlohmann::json& event_data = it.value();

        uint16_t event_id = event_data["event_id"];

        simba::database::objects::EventElement event_element(event_id);

        for (const auto& subscriber : event_data["subscribers"]) {
            uint16_t service_id = subscriber["service_id"];
            std::string name = subscriber["interface"]["name"]; // Where to save?
            std::string ip = subscriber["interface"]["ip"];
            uint16_t port = subscriber["interface"]["port"];

            simba::database::objects::ServiceElement service_element(service_id, ip, port);
            event_element.AddService(service_element);
        }

        pub_events.emplace(event_key, event_element);
    }

    return pub_events;
}

std::unordered_map<std::string, simba::database::objects::MethodElement> Json_parser::parseDb(const nlohmann::json& data) {
    std::unordered_map<std::string, simba::database::objects::MethodElement> db;

    for (auto it = data["db"].begin(); it != data["db"].end(); ++it) {
        const std::string& key = it.key();
        const nlohmann::json& method_data = it.value();


        uint16_t method_id = method_data["method_id"];
        uint16_t service_id = method_data["service_id"];
        std::string name = method_data["interface"]["name"]; // Where to store?
        std::string ip_address = method_data["interface"]["ip"];
        uint16_t port = method_data["service"]["port"];

        simba::database::objects::ServiceElement service_element(service_id, ip_address, port);
        simba::database::objects::MethodElement method_element(method_id, service_element);

        db.emplace(key, method_element);
    }

    return db;
}



std::unordered_map<std::string, std::string> Json_parser::parseConfig(const nlohmann::json& json_data) {
    std::unordered_map<std::string, std::string> conf;

    for (auto it = json_data["conf"].begin(); it != json_data["conf"].end(); ++it) {
        const std::string& key = it.key();
        const std::string& value = it.value();
        conf[key] = value;
    }

    return conf;
}

} // namespace json
} // namespace database
} // namespace simba
