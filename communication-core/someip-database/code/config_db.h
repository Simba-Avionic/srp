/**
 * @file config_db.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_CONFIG_DB_H_
#define COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_CONFIG_DB_H_

#include <string>
#include <unordered_map>
#include "communication-core/someip-database/code/interface.h"
#include "communication-core/someip-database/code/endpoint.h"

namespace simba {
namespace com {
namespace someip {
namespace objects {
    
class ConfigDb {
 private:
    std::unordered_map<std::string, Endpoint> item_list{};
 public:
  ConfigDb(/* args */);
  ~ConfigDb();
};
}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace simba

#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_CONFIG_DB_H_
