/**
 * @file ipc_socket.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "communication-core/sockets/ipc_socket.h"

#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

namespace simba {
namespace com {
namespace soc {
bool IpcSocket::SocketExist(const std::string path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

simba::core::ErrorCode IpcSocket::Init(const SocketConfig& config) {
  memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
  server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
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

void IpcSocket::SetRXCallback(RXCallback callback) {
  this->callback_ = callback;
}

simba::core::ErrorCode IpcSocket::Transmit(const std::string& ip,
                                           const std::uint16_t port,
                                           std::vector<std::uint8_t> payload) {
  int client_socket, rc;
  struct sockaddr_un remote;
  memset(&remote, 0, sizeof(struct sockaddr_un));
  client_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (client_socket == -1) {
    return simba::core::ErrorCode::kError;
  }

  remote.sun_family = AF_UNIX;
  strcpy(remote.sun_path, ("/run/" + ip).c_str());  // NOLINT

  std::uint8_t* buffor = new std::uint8_t[payload.size()];
  std::copy(payload.begin(), payload.end(), buffor);

  rc = sendto(client_socket, buffor, payload.size(), 0,
              (struct sockaddr*)&remote, sizeof(remote));
  delete[] buffor;
  close(client_socket);
  if (rc == -1) {
    return simba::core::ErrorCode::kError;
  }

  return simba::core::ErrorCode::kOk;
}

void IpcSocket::StartRXThread() {
  if (rx_thred != nullptr) {
    return;
  }
  this->rx_thred = std::make_unique<std::jthread>(
      [&](std::stop_token stoken) { this->Loop(stoken); });
  pthread_setname_np(this->rx_thred->native_handle(), "IpcSocket_RX_Thread");
}

void IpcSocket::Loop(std::stop_token stoken) {
  rc = bind(server_sock, (struct sockaddr*)&server_sockaddr, len);
  if (rc == -1) {
    return;
  }
  std::stop_callback stop_wait{stoken, [this]() { close(this->server_sock); }};
  while (true) {
    std::array<char, 256 * 2> buffor;
    bytes_rec =
        read(server_sock, reinterpret_cast<char*>(&buffor), 256 * 2);  // NOLINT
    if (bytes_rec >= 0) {
      if (this->callback_) {
        if (buffor.size() > 0) {
          this->callback_(
              "IPC", 0,
              std::vector<uint8_t>{buffor.begin(), buffor.begin() + bytes_rec});
        }
      }
    }
    if (stoken.stop_requested()) {
      break;
    }
  }
  // close(server_sock);
}

void IpcSocket::StopRXThread() {
  this->rx_thred->request_stop();
  this->rx_thred->join();
}
IpcSocket::~IpcSocket() {
  this->rx_thred->request_stop();
  this->rx_thred->join();
}
}  // namespace soc
}  // namespace com
}  // namespace simba
