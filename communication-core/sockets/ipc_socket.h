/**
 * @file ipc_socket.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOCKETS_IPC_SOCKET_H_
#define COMMUNICATION_CORE_SOCKETS_IPC_SOCKET_H_
#include <errno.h>
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
class IpcSocket : public ISocket {
 private:
  int server_sock, len, rc;
  int bytes_rec = 0;
  struct sockaddr_un server_sockaddr, peer_sock;


  std::unique_ptr<std::jthread> rx_thred;
  RXCallback callback_;
  bool SocketExist(const std::string path);
  void Loop(std::stop_token stoken);

 public:
  /**
   * @brief Socket init function
   *
   * @param config Config file
   * @return srp::core::ErrorCode initialiaze status
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
   * @return srp::core::ErrorCode status
   */
  srp::core::ErrorCode Transmit(const std::string& ip,
                                  const std::uint16_t port,
                                  std::vector<std::uint8_t> payload) override;
  /**
   * @brief This function start RX thread
   *
   */
  ~IpcSocket();
  void StartRXThread() override;
  void StopRXThread() override;
};
}  // namespace soc
}  // namespace com
}  // namespace srp

#endif  // COMMUNICATION_CORE_SOCKETS_IPC_SOCKET_H_
