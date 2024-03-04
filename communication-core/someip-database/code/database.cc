/**
 * @file database.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "communication-core/someip-database/code/database.h"

#include <algorithm>
namespace simba {
namespace com {
namespace someip {
namespace objects {
core::ErrorCode DataBase::InsertService(const uint16_t service_id,
                                        const Interface& inf) noexcept {
  if (list.find(service_id) != list.end()) {
    return core::ErrorCode::kError;
  }
  this->list.insert({service_id, inf});
  return core::ErrorCode::kOk;
}
std::optional<Interface> DataBase::FindService(
    const uint16_t service_id) const noexcept {
  const auto obj = list.find(service_id);
  if (obj == list.end()) {
    return {};
  }

  return obj->second;
}
}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace simba
