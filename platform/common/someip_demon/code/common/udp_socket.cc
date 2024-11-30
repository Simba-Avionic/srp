/**
 * @file udp_socket.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/someip_demon/code/common/udp_socket.h"

#include <arpa/inet.h>

#include <algorithm>
#include <future>  // NOLINT
#include <iostream>
#include <string>
#include <vector>

#include "ara/log/log.h"
#include "unistd.h"
namespace simba {
namespace common {
namespace soc {

namespace {
constexpr uint32_t kBufforSize{255 * 2};
}  // namespace

simba::core::ErrorCode UdpSocket::Init(const std::string &local_ip,
                                       const std::uint16_t port_id) {
  local_ip_ = local_ip;
  port_id_ = port_id;

  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0) {
    return simba::core::ErrorCode::kError;
  }

  srcaddr.sin_family = AF_INET;
  srcaddr.sin_addr.s_addr = inet_addr(local_ip.c_str());
  srcaddr.sin_port = htons(port_id_);

  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF,
                 (char *)&srcaddr,  // NOLINT
                 sizeof(srcaddr)) < 0) {
    return simba::core::ErrorCode::kError;
  }
  {
    int reuse = 1;

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse,  // NOLINT
                   sizeof(reuse)) < 0) {
      return simba::core::ErrorCode::kError;
    }
  }

  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) {
    return simba::core::ErrorCode::kError;
  }

  memset((char *)&localSock, 0, sizeof(localSock));  // NOLINT
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(port_id_);
  localSock.sin_addr.s_addr = INADDR_ANY;

  if (bind(sd, (struct sockaddr *)&localSock, sizeof(localSock))) {
    close(sd);
    return simba::core::ErrorCode::kError;
  }

  return simba::core::ErrorCode::kOk;
}

void UdpSocket::SetRXCallback(RXCallback callback) {
  this->callback_ = callback;
}

void UdpSocket::Transmit(const uint32_t &ip, const uint16_t port,
                         const std::vector<std::uint8_t> &payload) {
  std::lock_guard<std::mutex> lock{sending_m_};
  struct sockaddr_in remote;
  memset(&remote, 0, sizeof(struct sockaddr_in));
  remote.sin_family = AF_INET;
  remote.sin_addr.s_addr = ip;
  remote.sin_port = htons(port);

  std::uint8_t *buffor = new std::uint8_t[payload.size()];
  std::copy(payload.begin(), payload.end(), buffor);

  if (sendto(sd, buffor, payload.size(), 0, (struct sockaddr *)&remote,
             sizeof(remote)) < 0) {
    // delete[] buffor;
    // return simba::core::ErrorCode::kError;
  }
  delete[] buffor;
  // return simba::core::ErrorCode::kOk;
}

void UdpSocket::Transmit(const std::string &ip, const uint16_t port,
                         const std::vector<std::uint8_t> &payload) {
  this->Transmit(inet_addr(ip.c_str()), port, payload);
}

void UdpSocket::StartRXThread() {
  if (rx_thread != nullptr) {
    return;
  }
  this->rx_thread = std::make_unique<std::jthread>(
      [&](std::stop_token stoken) { this->Loop(stoken); });
  pthread_setname_np(this->rx_thread->native_handle(), "UdpSocket_RX_Thread");
}

void UdpSocket::Loop(std::stop_token stoken) {
  const std::stop_callback stop_wait{stoken,
                               [this]() { shutdown(this->sd, SHUT_RD); }};
  while (!stoken.stop_requested()) {
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    std::array<char, kBufforSize> buffor;
    const int32_t bytes_rec =
        recvfrom(sd, buffor.data(), kBufforSize, 0,
                 (struct sockaddr *)&si_other, (socklen_t *)&slen);  // NOLINT
    if (bytes_rec > 0) {
      if (this->callback_) {
        std::async(
            std::launch::async, [this, &si_other, &bytes_rec, &buffor]() {
              this->callback_(std::string(inet_ntoa(si_other.sin_addr)),
                              htons(si_other.sin_port),
                              std::vector<uint8_t>{buffor.begin(),
                                                   buffor.begin() + bytes_rec});
            });
      }
    } else {
      std::cout << "Run empty" << std::endl;
    }
  }
}
}  //  namespace soc
}  // namespace common
}  //  namespace simba
