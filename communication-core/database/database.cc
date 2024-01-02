/**
 * @file database.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "database.h"
namespace simba {
namespace database {
simba::core::Result<DatabaseElement> Database::GetService(
    const std::uint16_t service_id) {
  auto obj = this->db.find(service_id);
  if (obj == this->db.end()) {
    return simba::core::Result<DatabaseElement>{};
  }
  return simba::core::Result<DatabaseElement>{obj->second};
}
bool Database::ServiceExist(const std::uint16_t service_id) {
  const auto obj = this->db.find(service_id);
  return obj != this->db.end();
}
void Database::AddService(const std::uint16_t service_id,
                          const DatabaseElement& element) {
  if (this->ServiceExist(service_id)) {
    return;
  }
  this->db.insert({service_id, element});
}
DatabaseElement Database::CreatDatabaseElement(const std::string& ip,
                                               const int16_t port) {
  return DatabaseElement{ip, port};
}
}  // namespace database
}  // namespace simba
