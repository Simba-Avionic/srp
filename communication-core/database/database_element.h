/**
 * @file database_element.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief this file define database element
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_DATABASE_DATABASE_ELEMENT_H_
#define COMMUNICATION_CORE_DATABASE_DATABASE_ELEMENT_H_
#include <iostream>
#include <string>
namespace simba {
namespace database {
class DatabaseElement {
 private:
  const std::string ip_;
  const uint16_t port_;

 public:
  DatabaseElement(const std::string& ip, const uint16_t port)
      : ip_{ip}, port_{port} {}
  const inline std::string GetIp() { return ip_; }
  const inline uint16_t GetPort() { return port_; }
  ~DatabaseElement() = default;
};
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_DATABASE_DATABASE_ELEMENT_H_
