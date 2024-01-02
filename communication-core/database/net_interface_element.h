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
#ifndef COMMUNICATION_CORE_DATABASE_NET_INTERFACE_ELEMENT_H_
#define COMMUNICATION_CORE_DATABASE_NET_INTERFACE_ELEMENT_H_
#include <stdint.h>

#include <string>

namespace simba {
namespace database {
namespace objects {
class NetInterfaceElement {
 private:
  const uint16_t service_id_;
  const std::string ip_address_;
  const std::string ipc_address_;
  const uint16_t port_;

 public:
  uint16_t GetServiceId() const { return this->service_id_; }
  std::string GetIpAddress() const { return this->ip_address_; }
  std::string GetIpcAddress() const { return this->ipc_address_; }
  uint16_t GetPort() const { return this->port_; }
  /**
   * @brief Construct a new Net Interface Element object
   *
   * @param service_id client or service id number
   * @param ip_address ip address
   * @param port ip port or 0 if ipc
   */
  NetInterfaceElement(const uint16_t& service_id, const std::string& ip_address,
                      const uint16_t& port, const std::string& ipc_address)
      : service_id_{service_id},
        ip_address_{ip_address},
        ipc_address_{ipc_address},
        port_{port} {}
  NetInterfaceElement(const uint16_t& service_id,
                      const std::string& ipc_address)
      : service_id_{service_id},
        ip_address_{""},
        ipc_address_{ipc_address},
        port_{0x00} {}
  NetInterfaceElement(const uint16_t& service_id, const std::string& ip_address,
                      const uint16_t& port)
      : service_id_{service_id},
        ip_address_{ip_address},
        ipc_address_{""},
        port_{port} {}
};
}  // namespace objects
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_DATABASE_NET_INTERFACE_ELEMENT_H_
