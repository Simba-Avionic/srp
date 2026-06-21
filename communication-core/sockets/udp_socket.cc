/**
 * @file udp_socket.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "communication-core/sockets/udp_socket.h"

#include <algorithm>
#include <string>
#include <vector>

#include "iostream"
#include "unistd.h"

namespace srp {
namespace com {
namespace soc {

namespace {
  constexpr uint32_t kBufforSize{255 * 2};
}  // namespace

srp::core::ErrorCode UdpSocket::Init(const SocketConfig& config) {
  server_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_sock < 0) {
    return srp::core::ErrorCode::kError;
  }

  int reuse = 1;

  if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse,  // NOLINT
                sizeof(reuse)) < 0) {
    return srp::core::ErrorCode::kError;
  }

  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  if (setsockopt(server_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) {
    return srp::core::ErrorCode::kError;
  }

  memset((char *)&server_sockaddr, 0, sizeof(server_sockaddr));  // NOLINT
  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_addr.s_addr = inet_addr(config.GetIp().c_str());
  server_sockaddr.sin_port = htons(config.GetRxPort());

  if (bind(server_sock, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr))) {
    close(server_sock);
    return srp::core::ErrorCode::kError;
  }

  return srp::core::ErrorCode::kOk;
}

void UdpSocket::SetRXCallback(RXCallback callback) {
  this->callback_ = callback;
}

srp::core::ErrorCode UdpSocket::Transmit(const std::string& ip,
                                           const std::uint16_t port,
                                           std::vector<std::uint8_t> payload) {
struct sockaddr_in remote;
memset(&remote, 0, sizeof(struct sockaddr_in));
remote.sin_family = AF_INET;
remote.sin_addr.s_addr = inet_addr(ip.c_str());
remote.sin_port = htons(port);

std::uint8_t *buffor = new std::uint8_t[payload.size()];
std::copy(payload.begin(), payload.end(), buffor);

  if (sendto(server_sock, buffor, payload.size(), 0, (struct sockaddr *)&remote,
           sizeof(remote)) < 0) {
    delete[] buffor;
    return srp::core::ErrorCode::kError;
  }
  delete[] buffor;
  return srp::core::ErrorCode::kOk;
}

void UdpSocket::StartRXThread() {
  if (rx_thread != nullptr) {
    return;
  }
  this->rx_thread = std::make_unique<std::jthread>(
      [&](std::stop_token stoken) { this->Loop(stoken); });
}

void UdpSocket::Loop(std::stop_token stoken) {
  const std::stop_callback stop_wait{stoken,
                             [this]() { shutdown(this->server_sock, SHUT_RD); }};
  while (!stoken.stop_requested()) {
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    std::array<char, kBufforSize> buffor;
    const int32_t bytes_rec =
        recvfrom(server_sock, buffor.data(), kBufforSize, 0,
                (struct sockaddr *)&si_other, (socklen_t *)&slen);  // NOLINT
    if (bytes_rec > 0) {
      if (this->callback_) {
        this->callback_(std::string(inet_ntoa(si_other.sin_addr)),
                              ntohs(si_other.sin_port),
                              std::vector<uint8_t>{buffor.begin(),
                                                  buffor.begin() + bytes_rec});
      }
    }
}
  close(server_sock);
}
}  //  namespace soc
}  //  namespace com
}  //  namespace srp
