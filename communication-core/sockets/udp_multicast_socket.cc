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

#include "communication-core/sockets/udp_multicast_socket.h"

#include <algorithm>
#include <string>
#include <vector>

#include "iostream"
#include "unistd.h"

namespace srp {
namespace com {
namespace soc {

srp::core::ErrorCode UdpMulticastSocket::Init(const SocketConfig &config) {
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0) {
    return srp::core::ErrorCode::kError;
  }

  memset((char *)&groupSock, 0, sizeof(groupSock));  // NOLINT
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr("231.255.42.99");
  groupSock.sin_port = htons(config.GetRxPort());
  {
    char loopch = 0;

    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP,
                   (char *)&loopch,  // NOLINT
                   sizeof(loopch)) < 0) {
      close(sd);
      return srp::core::ErrorCode::kError;
    }
  }
  localInterface.s_addr = inet_addr(config.GetIp().c_str());
  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF,
                 (char *)&localInterface,  // NOLINT
                 sizeof(localInterface)) < 0) {
    return srp::core::ErrorCode::kError;
  }
  return srp::core::ErrorCode::kOk;
}

void UdpMulticastSocket::SetRXCallback(RXCallback callback) {}

srp::core::ErrorCode UdpMulticastSocket::Transmit(
    const std::string &ip, const std::uint16_t port,
    std::vector<std::uint8_t> payload) {
  std::uint8_t *buffor = new std::uint8_t[payload.size()];
  std::copy(payload.begin(), payload.end(), buffor);
  if (sendto(sd, buffor, payload.size(), 0, (struct sockaddr *)&groupSock,
             sizeof(groupSock)) < 0) {
    delete[] buffor;
    return srp::core::ErrorCode::kError;
  }
  delete[] buffor;
  return srp::core::ErrorCode::kOk;
}

void UdpMulticastSocket::StartRXThread() {}

void UdpMulticastSocket::Loop(std::stop_token stoken) {}
}  //  namespace soc
}  //  namespace com
}  //  namespace srp
