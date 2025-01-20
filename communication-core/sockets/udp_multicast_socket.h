/**
 * @file udp_multicast_socket.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOCKETS_UDP_MULTICAST_SOCKET_H_
#define COMMUNICATION_CORE_SOCKETS_UDP_MULTICAST_SOCKET_H_
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "communication-core/sockets/Isocket.h"
namespace srp {
namespace com {
namespace soc {
class UdpMulticastSocket : public ISocket {
 private:
  struct in_addr localInterface;
  struct sockaddr_in groupSock;
  int sd;
  //   int server_sock, len;
  //   int bytes_rec = 0;
  //   struct sockaddr_in server_sockaddr, peer_sock;

  //   std::unique_ptr<std::jthread> rx_thread;
  void Loop(std::stop_token stoken);
  //   RXCallback callback_;

 public:
  /**
   * @brief Socket init function
   *
   * @param config Config file
   * @return core::ErrorCode initialiaze status
   */
  srp::core::ErrorCode Init(const SocketConfig& config) override;
  /**
   * @brief Setter for rx callback
   *
   * @param callback
   */
  void SetRXCallback(RXCallback callback) override;
  /**
   * @brief Function to send data by socket
   *
   * @param ip target ip or path
   * @param port target port or 0 for ipcs
   * @param payload payload to send
   * @return core::ErrorCode status
   */
  srp::core::ErrorCode Transmit(const std::string& ip,
                                  const std::uint16_t port,
                                  std::vector<std::uint8_t> payload) override;
  /**
   * @brief This function start RX thread
   *
   */
  void StartRXThread() override;
  UdpMulticastSocket() = default;
};
}  // namespace soc
}  // namespace com
}  // namespace srp

#endif  // COMMUNICATION_CORE_SOCKETS_UDP_MULTICAST_SOCKET_H_
