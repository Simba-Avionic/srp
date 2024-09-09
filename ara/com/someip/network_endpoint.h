/**
 * @file network_endpoint.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_NETWORK_ENDPOINT_H_
#define ARA_COM_SOMEIP_NETWORK_ENDPOINT_H_
#include <cstdint>
#include <string>
#include <variant>

namespace ara {
namespace com {
class NetworkEndpoint {
 public:
  const std::uint16_t remote_port_;
  const std::string ip_;
  NetworkEndpoint(std::uint16_t remote_port, const std::string& ip)
      : remote_port_{remote_port}, ip_{ip} {}
  ~NetworkEndpoint() = default;
};
}  // namespace com

}  // namespace ara

#endif  // ARA_COM_SOMEIP_NETWORK_ENDPOINT_H_
