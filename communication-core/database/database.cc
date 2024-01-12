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
#include "communication-core/database/database.h"

#include "communication-core/database/method_element.h"
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

DatabaseElement Database::CreatDbElement(const std::string& ip,
                                         const uint16_t port) {
  return DatabaseElement{ip, port};
}
objects::MethodElement Database::CreatReqMethodElement(
    const uint16_t& method_id, const uint16_t& service_id) {
  return objects::MethodElement(method_id, service_id);
}

void Database::AddDbElement(const uint16_t& key, DatabaseElement element) {
  if (this->db.find(key) == this->db.end()) {
    this->db.insert({key, element});
  }
}

void Database::AddReqMethodsElement(const std::string& key,
                                    objects::MethodElement element) {
  if (this->req_methods.find(key) == this->req_methods.end()) {
    this->req_methods.insert({key, element});
  }
}

void Database::AddPubMethodElement(const std::string& key, const uint16_t& id) {
  if (this->pub_method.find(key) == this->pub_method.end()) {
    this->pub_method.insert({key, id});
  }
}

}  // namespace database
}  // namespace simba
