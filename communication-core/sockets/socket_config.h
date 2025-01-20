/**
 * @file socket_config.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOCKETS_SOCKET_CONFIG_H_
#define COMMUNICATION_CORE_SOCKETS_SOCKET_CONFIG_H_
#include <cstdint>
#include <string>

namespace srp {
namespace com {
namespace soc {
class SocketConfig {
 private:
  const std::string ip_;
  const std::uint16_t rx_port_;
  const std::uint16_t tx_port_;

 public:
  SocketConfig(const std::string& ip, const std::uint16_t rx_port,
               const std::uint16_t tx_port)
      : ip_{ip}, rx_port_{rx_port}, tx_port_{tx_port} {}
  SocketConfig(const SocketConfig& other)
      : ip_{other.ip_}, rx_port_{other.rx_port_}, tx_port_{other.tx_port_} {}
  SocketConfig operator=(const srp::com::soc::SocketConfig& other) {
    return SocketConfig{other};
  }
  const std::string GetIp() const { return this->ip_; }
  const std::uint16_t GetRxPort() const { return this->rx_port_; }
};
}  // namespace soc
}  // namespace com
}  // namespace srp
#endif  // COMMUNICATION_CORE_SOCKETS_SOCKET_CONFIG_H_
