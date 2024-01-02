/**
 * @file service_element.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_DATABASE_SERVICE_ELEMENT_H_
#define COMMUNICATION_CORE_DATABASE_SERVICE_ELEMENT_H_
#include <stdint.h>

#include <string>

namespace simba {
namespace database {
namespace objects {
class ServiceElement {
 private:
  const uint16_t service_id_;
  const std::string ip_address_;
  const uint16_t port_;

 public:
  uint16_t GetServiceId() const { return this->service_id_; }
  std::string GetIpAddress() const { return this->ip_address_; }
  uint16_t GetPort() const { return this->port_; }
  /**
   * @brief Construct a new Service Element object
   *
   * @param service_id client or service id number
   * @param ip_address ip address or ipc address
   * @param port ip port or 0 if ipc
   */
  ServiceElement(const uint16_t& service_id, const std::string& ip_address,
                 const uint16_t& port)
      : service_id_{service_id}, ip_address_{ip_address}, port_{port} {}
  /**
   * @brief Construct a new Service Element object
   *
   * @param service_id client or service id number
   * @param ip_address ipc address
   */
  ServiceElement(const uint16_t& service_id, const std::string& ip_address)
      : service_id_{service_id}, ip_address_{ip_address}, port_{0x00} {}
};
}  // namespace objects
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_DATABASE_SERVICE_ELEMENT_H_
