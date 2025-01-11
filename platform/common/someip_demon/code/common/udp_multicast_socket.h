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
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_UDP_MULTICAST_SOCKET_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_UDP_MULTICAST_SOCKET_H_
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>

#include <functional>
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "core/common/error_code.h"

namespace srp {
namespace common {
namespace soc {
class UdpMulticastSocket {
 public:
  using RXCallback =
      std::function<void(const std::string& ip, const std::uint16_t& port,
                         const std::vector<std::uint8_t>&)>;

 private:
  struct in_addr localInterface;
  struct sockaddr_in groupSock;
  struct sockaddr_in localSock;
  struct ip_mreq group;
  struct sockaddr_in srcaddr;
  struct sockaddr_in cliaddr;
  int sd;
  std::string local_ip_;
  std::string multicast_ip_;
  std::uint16_t port_id_;

  int s;                        /* s = socket */
  struct sockaddr_in in_addr;   /* Structure used for bind() */
  struct sockaddr_in sock_addr; /* Output structure from getsockname */
  struct sockaddr_in src_addr;  /* Used to receive (addr,port) of sender */
  int src_addr_len;             /* Length of src_addr */
  int len;                      /* Length of result from getsockname */
  int mc_addr, port;
  struct ip_mreq mreq;
  struct hostent* host_entry_ptr;
  char line[100];

  //   int server_sock, len;
  //   int bytes_rec = 0;
  //   struct sockaddr_in server_sockaddr, peer_sock;

  std::unique_ptr<std::jthread> rx_thread;
  void Loop(std::stop_token stoken);
  RXCallback callback_;

 public:
  /**
   * @brief Socket init function
   *
   * @param config Config file
   * @return core::ErrorCode initialiaze status
   */
  srp::core::ErrorCode Init(const std::string& local_ip,
                              const std::string& multicast_ip,
                              const std::uint16_t port_id);
  /**
   * @brief Setter for rx callback
   *
   * @param callback
   */
  void SetRXCallback(RXCallback callback);
  /**
   * @brief Function to send data by socket
   *
   * @param ip target ip or path
   * @param port target port or 0 for ipcs
   * @param payload payload to send
   * @return core::ErrorCode status
   */
  void Transmit(const std::vector<std::uint8_t>& payload);
  /**
   * @brief This function start RX thread
   *
   */
  void StartRXThread();
  UdpMulticastSocket() = default;
};
}  // namespace soc
}  // namespace common
}  // namespace srp

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_UDP_MULTICAST_SOCKET_H_
