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

core::ErrorCode DataBase::InstertEvent(const uint16_t event_id,
                                       const uint16_t client_id) noexcept {
  auto obj = this->event_map.find(event_id);
  if (obj == this->event_map.end()) {
    this->event_map.insert({event_id, std::vector<uint16_t>{client_id}});
    return core::ErrorCode::kOk;
  }
  if (std::find(obj->second.begin(), obj->second.end(), client_id) ==
      obj->second.end()) {
    obj->second.push_back(client_id);
    return core::ErrorCode::kOk;
  }
  return core::ErrorCode::kError;
}
std::optional<std::vector<uint16_t>> DataBase::FindEventClient(
    const uint16_t event_id) const noexcept {
  auto obj = this->event_map.find(event_id);
  if (obj == this->event_map.end()) {
    return {};
  } else {
    return obj->second;
  }
}

core::ErrorCode DataBase::InsertInterface(const std::string& ip,
                                          const uint16_t port) noexcept {
  this->interfaces.push_back(objects::Interface{ip, port});
  return core::ErrorCode::kOk;
}
core::ErrorCode DataBase::SetServiceId(const uint16_t id) noexcept {
  this->service_id = id;
  return core::ErrorCode::kOk;
}
uint16_t DataBase::GetServiceId() const noexcept { return this->service_id; }

std::vector<objects::Interface> DataBase::GetInterfaces() const noexcept {
  return this->interfaces;
}
}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace simba
