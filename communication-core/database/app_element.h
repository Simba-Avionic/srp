/**
 * @file app_element.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_DATABASE_APP_ELEMENT_H_
#define COMMUNICATION_CORE_DATABASE_APP_ELEMENT_H_

#include <iostream>

#include "communication-core/database/net_interface_element.h"
#include "communication-core/database/method_element.h"
#include "communication-core/database/event_element.h"
#include "communication-core/database/req_event_element.h"
#include <string>

#include "database/event_element.h"
#include "database/method_element.h"
#include "database/net_interface_element.h"
#include "database/req_event_element.h"
#include <unordered_map>

namespace simba {
namespace database {
namespace objects {
class AppElement {
 private:
  std::string name;
  NetInterfaceElement net_interface;
  std::unordered_map<std::string, std::string> pub_methods;
  std::unordered_map<std::string, ReqEventElement> req_events;
  std::unordered_map<std::string, EventElement> pub_events;
  std::unordered_map<std::string, MethodElement> db;
  std::unordered_map<std::string, std::string> conf;

 public:
  AppElement(
      const std::string& app_name,
      const NetInterfaceElement& net_interface_element,
      const std::unordered_map<std::string, std::string>& pub_methods_map,
      const std::unordered_map<std::string, ReqEventElement>& req_events_map,
      const std::unordered_map<std::string, EventElement>& pub_events_map,
      const std::unordered_map<std::string, MethodElement>& db_map,
      const std::unordered_map<std::string, std::string>& conf_map)
      : name(app_name),
        net_interface(net_interface_element),
        pub_methods(pub_methods_map),
        req_events(req_events_map),
        pub_events(pub_events_map),
        db(db_map),
        conf(conf_map) {}
  ~AppElement() = default;
  friend std::ostream& operator<<(std::ostream& os,
                                  const AppElement& app_element);
};

std::ostream& operator<<(std::ostream& os, const AppElement& app_element) {
  os << "App Name: " << app_element.name << "\n";
  os << "Net Interface: " << app_element.net_interface << "\n";

  os << "Pub Methods:\n";
  for (const auto& entry : app_element.pub_methods) {
    os << "  " << entry.first << ": " << entry.second << "\n";
  }

  os << "Req Events:\n";
  for (const auto& entry : app_element.req_events) {
    os << "  " << entry.first << ": " << entry.second << "\n";
  }

  os << "Pub Events:\n";
  for (const auto& entry : app_element.pub_events) {
    os << "  " << entry.first << ": " << entry.second << "\n";
  }

  os << "Database Methods:\n";
  for (const auto& entry : app_element.db) {
    os << "  " << entry.first << ": " << entry.second << "\n";
  }

  os << "Configuration:\n";
  for (const auto& entry : app_element.conf) {
    os << "  " << entry.first << ": " << entry.second << "\n";
  }

  return os;
}
}  // namespace objects
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_DATABASE_APP_ELEMENT_H_
