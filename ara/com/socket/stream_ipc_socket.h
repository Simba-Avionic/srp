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
#ifndef ARA_COM_SOCKET_STREAM_IPC_SOCKET_H_
#define ARA_COM_SOCKET_STREAM_IPC_SOCKET_H_
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

#include "ara/com/socket/Isocket_stream.h"
namespace ara {
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
  ara::core::Result<void> Init(const std::string& socket_path) override;
  /**
   * @brief Setter for rx callback
   *
   * @param callback
   */
  void SetRXCallback(RXCallbackStream&& callback) override;
  /**
   * @brief Function to send data by socket
   *
   * @param ip target ip or path
   * @param port target port or 0 for ipcs
   * @param payload payload to send
   * @return std::optional<std::vector<uint8_t>> response
   */
  std::optional<std::vector<uint8_t>> Transmit(
      const std::string& socket_path,
      std::vector<std::uint8_t> payload) override;
  /**
   * @brief This function start RX thread
   *
   */
  void StartRXThread() override;
  void StopRXThread() override;
};
}  // namespace soc
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOCKET_STREAM_IPC_SOCKET_H_
