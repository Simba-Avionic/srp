/**
 * @file config_db.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "communication-core/someip-database/code/config_db.h"
namespace simba {
namespace com {
namespace someip {
namespace objects {
simba::core::ErrorCode ConfigDb::InsertObject(const std::string& key,
                                       const Endpoint& item) noexcept {
  if (this->item_list.find(key) != this->item_list.end()) {
    return core::ErrorCode::kError;
  }

  this->item_list.insert({key, item});
  return core::ErrorCode::kOk;
}

std::optional<Endpoint> ConfigDb::FindObject(
    const std::string& key) const noexcept {
  const auto obj = this->item_list.find(key);
  if (obj == this->item_list.end()) {
    return {};
  }
  return obj->second;
}

ConfigDb::ConfigDb(/* args */) {}

ConfigDb::~ConfigDb() {}
}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace simba
