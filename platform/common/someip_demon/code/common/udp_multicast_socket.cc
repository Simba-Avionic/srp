/**
 * @file udp_multicast_socket.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "platform/common/someip_demon/code/common/udp_multicast_socket.h"

#include <arpa/inet.h>

#include <algorithm>
#include <future>  // NOLINT
#include <iostream>
#include <string>
#include <vector>

#include "unistd.h"
namespace srp {
namespace common {
namespace soc {

namespace {
constexpr uint32_t kBufforSize{255 * 2};
}  // namespace

srp::core::ErrorCode UdpMulticastSocket::Init(const std::string &local_ip,
                                                const std::string &multicast_ip,
                                                const std::uint16_t port_id) {
  local_ip_ = local_ip;
  multicast_ip_ = multicast_ip;
  port_id_ = port_id;

  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0) {
    return srp::core::ErrorCode::kError;
  }

  memset((char *)&groupSock, 0, sizeof(groupSock));  // NOLINT
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr(multicast_ip.c_str());
  groupSock.sin_port = htons(port_id);
  // {
  //   char loopch = 0;

  //   if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP,
  //                  (char *)&loopch,  // NOLINT
  //                  sizeof(loopch)) < 0) {
  //     close(sd);
  //     return srp::core::ErrorCode::kError;
  //   }
  // }

  srcaddr.sin_family = AF_INET;
  srcaddr.sin_addr.s_addr = inet_addr(local_ip.c_str());
  srcaddr.sin_port = htons(port_id_);

  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF,
                 (char *)&srcaddr,  // NOLINT
                 sizeof(srcaddr)) < 0) {
    return srp::core::ErrorCode::kError;
  }
  {
    int reuse = 1;

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse,  // NOLINT
                   sizeof(reuse)) < 0) {
      return srp::core::ErrorCode::kError;
    }
  }

  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) {
    return srp::core::ErrorCode::kError;
  }

  memset((char *)&localSock, 0, sizeof(localSock));  // NOLINT
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(port_id_);
  localSock.sin_addr.s_addr = INADDR_ANY;

  if (bind(sd, (struct sockaddr *)&localSock, sizeof(localSock))) {
    close(sd);
    return srp::core::ErrorCode::kError;
  }

  group.imr_multiaddr.s_addr = inet_addr(multicast_ip_.c_str());
  group.imr_interface.s_addr = inet_addr(local_ip_.c_str());
  if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group,  // NOLINT
                 sizeof(group)) < 0) {
    close(sd);
    return srp::core::ErrorCode::kError;
  }
  return srp::core::ErrorCode::kOk;
}

void UdpMulticastSocket::SetRXCallback(RXCallback callback) {
  this->callback_ = callback;
}

void UdpMulticastSocket::Transmit(const std::vector<std::uint8_t> &payload) {
  std::uint8_t *buffor = new std::uint8_t[payload.size()];
  std::copy(payload.begin(), payload.end(), buffor);
  if (sendto(sd, buffor, payload.size(), 0, (struct sockaddr *)&groupSock,
             sizeof(groupSock)) < 0) {
    delete[] buffor;
    // return srp::core::ErrorCode::kError;
  }
  delete[] buffor;
  // return srp::core::ErrorCode::kOk;
}

void UdpMulticastSocket::StartRXThread() {
  if (rx_thread != nullptr) {
    return;
  }
  this->rx_thread = std::make_unique<std::jthread>(
      [&](std::stop_token stoken) { this->Loop(stoken); });
  pthread_setname_np(this->rx_thread->native_handle(),
                     "UdpMulticastSocket_RX_Thread");
}

void UdpMulticastSocket::Loop(std::stop_token stoken) {
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
        std::ignore = std::async(
            std::launch::async, [this, &si_other, &bytes_rec, &buffor]() {
              this->callback_(std::string(inet_ntoa(si_other.sin_addr)),
                              htons(si_other.sin_port),
                              std::vector<uint8_t>{buffor.begin(),
                                                   buffor.begin() + bytes_rec});
            });
      }
    }
  }
}
}  //  namespace soc
}  // namespace common
}  //  namespace srp
