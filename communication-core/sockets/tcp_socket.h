/**
 * @file tcp_socket.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_SOCKETS_TCP_SOCKET_H_
#define COMMUNICATION_CORE_SOCKETS_TCP_SOCKET_H_

#include <netinet/in.h>
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <vector>
#include <cstring>

#include "communication-core/sockets/Isocket_stream.h"
namespace srp {
namespace com {
namespace soc {
class StreamTCPSocket : public ISocketStream {
 private:
  int server_sock, len, rc;
  int bytes_rec = 0;
  sockaddr_in server_addr;

  std::unique_ptr<std::jthread> rx_thred;
  void Loop(std::stop_token stoken);
  RXCallbackStream callback_;

 public:
  StreamTCPSocket() = default;

  srp::core::ErrorCode Init(const SocketConfig& config) override;
  std::optional<std::vector<uint8_t>> Transmit(const std::string& ip,
                                    const std::uint16_t port,
                                    std::vector<std::uint8_t> payload) override;
  void SetRXCallback(RXCallbackStream callback) override;
  void StartRXThread() override;
};

}  // namespace soc
}  // namespace com
}  // namespace srp

#endif  // COMMUNICATION_CORE_SOCKETS_TCP_SOCKET_H_
