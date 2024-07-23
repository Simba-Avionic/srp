/**
 * @file stream_ipc_socket.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_SOCKETS_STREAM_IPC_SOCKET_H_
#define COMMUNICATION_CORE_SOCKETS_STREAM_IPC_SOCKET_H_
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

#include "communication-core/sockets/Isocket_stream.h"
namespace simba {
namespace com {
namespace soc {
class StreamIpcSocket : public ISocketStream {
 private:
  int server_sock, len, rc;
  int bytes_rec = 0;
  struct sockaddr_un server_sockaddr, peer_sock;


  std::unique_ptr<std::jthread> rx_thred;
  void Loop(std::stop_token stoken);
  RXCallbackStream callback_;
  bool SocketExist(const std::string path);

 public:
  StreamIpcSocket() = default;
  /**
   * @brief Socket init function
   *
   * @param config Config file
   * @return simba::core::ErrorCode initialiaze status
   */
  simba::core::ErrorCode Init(const SocketConfig& config) override;
  /**
   * @brief Setter for rx callback
   *
   * @param callback
   */
  void SetRXCallback(RXCallbackStream callback) override;
  /**
   * @brief Function to send data by socket
   *
   * @param ip target ip or path
   * @param port target port or 0 for ipcs
   * @param payload payload to send
   * @return std::optional<std::vector<uint8_t>> response
   */
  std::optional<std::vector<uint8_t>> Transmit(const std::string& ip,
                                  const std::uint16_t port,
                                  std::vector<std::uint8_t> payload) override;
  /**
   * @brief This function start RX thread
   *
   */
  void StartRXThread() override;
};
}  // namespace soc
}  // namespace com
}  // namespace simba

#endif  // COMMUNICATION_CORE_SOCKETS_STREAM_IPC_SOCKET_H_
