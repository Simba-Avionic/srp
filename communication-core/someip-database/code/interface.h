/**
 * @file interface.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_INTERFACE_H_
#define COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_INTERFACE_H_

#include <cstdint>
#include <string>
#include <utility>

namespace srp {
namespace com {
namespace someip {
namespace objects {
class Interface {
 private:
  const uint16_t port_;
  const std::string ip_;

 public:
  Interface(const std::string& ip, const uint16_t port)
      : port_{port}, ip_{ip} {}
  Interface(const Interface& other) : port_{other.port_}, ip_{other.ip_} {}
  Interface operator=(const Interface& other) {
    return std::move(Interface(other));
  }
  uint16_t GetPort() const { return this->port_; }
  std::string GetIp() const { return this->ip_; }
};
}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace srp

#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_INTERFACE_H_
