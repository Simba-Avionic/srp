/**
 * @file stream_ipc_socket.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "communication-core/sockets/stream_ipc_socket.h"

#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <vector>
namespace simba {
namespace com {
namespace soc {
bool StreamIpcSocket::SocketExist(const std::string path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

simba::core::ErrorCode StreamIpcSocket::Init(const SocketConfig& config) {
  memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
  server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (server_sock == -1) {
    return simba::core::ErrorCode::kInitializeError;
  }
  umask(0);
  server_sockaddr.sun_family = AF_UNIX;
  if (SocketExist("/run/" + config.GetIp())) {
    remove(("/run/" + config.GetIp()).c_str());
  }
  strcpy(server_sockaddr.sun_path,             // NOLINT
         ("/run/" + config.GetIp()).c_str());  // NOLINT
  len = sizeof(server_sockaddr);
  unlink(("/run/" + config.GetIp()).c_str());
  return simba::core::ErrorCode::kOk;
}

void StreamIpcSocket::SetRXCallback(RXCallbackStream callback) {
  this->callback_ = callback;
}

std::optional<std::vector<uint8_t>> StreamIpcSocket::Transmit(
    const std::string& ip, const std::uint16_t port,
    std::vector<std::uint8_t> payload) {
  const int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  struct timeval timeout;
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
  setsockopt(server_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  struct sockaddr_un server_addr;
  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, ("/run/" + ip).c_str());  // NOLINT
  if (connect(server_socket, (struct sockaddr*)&server_addr,
              sizeof(server_addr)) == -1) {
    close(server_sock);
    return {};
  }
  if (write(server_socket, payload.data(), payload.size()) < 0) {
    close(server_sock);
    return {};
  }
  std::array<char, 256 * 2> buffer;
  const auto size =
      read(server_socket, reinterpret_cast<char*>(&buffer), 256 * 2);
  close(server_socket);
  if (size < 0) {
    return {};
  }
  return std::vector<uint8_t>{buffer.begin(), buffer.begin() + size};
}

void StreamIpcSocket::StartRXThread() {
  if (rx_thred != nullptr) {
    return;
  }
  this->rx_thred = std::make_unique<std::jthread>(
      [&](std::stop_token stoken) { this->Loop(stoken); });
  pthread_setname_np(rx_thred->native_handle(), "StreamIpcSocket RX Thread");
}

void StreamIpcSocket::Loop(std::stop_token stoken) {
  sockaddr_un client_addr;
  int client_socket;
  rc = bind(server_sock, (struct sockaddr*)&server_sockaddr, len);
  std::stop_callback stopcall{stoken, [this]() { close(this->server_sock); }};
  if (rc == -1) {
    return;
  }
  listen(server_sock, 1);
  while (true) {
    std::array<char, 256 * 2> buffer;
    int clen = sizeof(client_addr);
    client_socket = accept(server_sock, (sockaddr*)&client_addr,  // NOLINT
                           (socklen_t*)&clen);                    // NOLINT
    if (client_socket < 0) {
      break;
    }
    bytes_rec = read(client_socket, reinterpret_cast<char*>(&buffer), 256 * 2);

    if (bytes_rec >= 0) {
      if (this->callback_) {
        if (buffer.size() > 0) {
          auto res = this->callback_(
              "IPC", 0,
              std::vector<uint8_t>{buffer.begin(), buffer.begin() + bytes_rec});
          write(client_socket, res.data(), res.size());
        }
      }
    }
    close(client_socket);
    if (stoken.stop_requested()) {
      break;
    }
  }
  // close(server_sock);
}
void StreamIpcSocket::StopRXThread() {
  this->rx_thred->request_stop();
  this->rx_thred->join();
}
}  // namespace soc
}  // namespace com
}  // namespace simba
