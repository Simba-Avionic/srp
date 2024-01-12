/**
 * @file database.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define database for router
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_DATABASE_DATABASE_H_
#define COMMUNICATION_CORE_DATABASE_DATABASE_H_
#include <cstdint>
#include <string>
#include <unordered_map>

#include "communication-core/database/Idatabase.h"
#include "communication-core/database/database_element.h"
#include "communication-core/database/method_element.h"
namespace simba {
namespace database {
class Database : public IDatabase {
 private:
  std::unordered_map<std::uint16_t, DatabaseElement> db{};
  std::unordered_map<std::string, objects::MethodElement> req_methods{};
  std::unordered_map<std::string, uint16_t> pub_method{};
 public:
  simba::core::Result<DatabaseElement> GetService(
      const std::uint16_t service_id) override;
  bool ServiceExist(const std::uint16_t service_id) override;
  DatabaseElement CreatDbElement(const std::string& ip,
                                 const uint16_t port) override;
  objects::MethodElement CreatReqMethodElement(
      const uint16_t& method_id, const uint16_t& service_id) override;
  void AddDbElement(const uint16_t& key, DatabaseElement element) override;

  void AddReqMethodsElement(const std::string& key,
                            objects::MethodElement element) override;

  void AddPubMethodElement(const std::string& key, const uint16_t& id) override;

 public:
};
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_DATABASE_DATABASE_H_
