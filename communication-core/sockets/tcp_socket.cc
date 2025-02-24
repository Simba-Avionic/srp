/**
 * @file tcp_socket.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "communication-core/sockets/tcp_socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <vector>
namespace srp {
namespace com {
namespace soc {

srp::core::ErrorCode StreamTCPSocket::Init(const SocketConfig& config) {
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        return srp::core::ErrorCode::kInitializeError;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config.GetRxPort());

    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        return srp::core::ErrorCode::kInitializeError;
    }

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        return srp::core::ErrorCode::kInitializeError;
    }

    return srp::core::ErrorCode::kOk;
}

void StreamTCPSocket::SetRXCallback(RXCallbackStream callback) {
    this->callback_ = callback;
}

std::optional<std::vector<uint8_t>> StreamTCPSocket::Transmit(const std::string& ip,
                                    const std::uint16_t port,
                                    std::vector<std::uint8_t> payload) {
    const int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
      return std::nullopt;
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &client_addr.sin_addr) <= 0) {
      close(client_socket);
      return std::nullopt;
    }

    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    if (connect(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
      close(client_socket);
      return std::nullopt;
    }

    if (write(client_socket, payload.data(), payload.size()) < 0) {
      close(client_socket);
      return std::nullopt;
    }

    std::array<char, 256 * 2> buffer;
    const auto size = read(client_socket, reinterpret_cast<char*>(&buffer), buffer.size());
    close(client_socket);
    if (size < 0) {
      return std::nullopt;
    }

    return std::vector<uint8_t>{buffer.begin(), buffer.begin() + size};
}

void StreamTCPSocket::StartRXThread() {
    if (rx_thred != nullptr) {
      return;
    }
    this->rx_thred = std::make_unique<std::jthread>(
        [&](std::stop_token stoken) { this->Loop(stoken); });
}

void StreamTCPSocket::Loop(std::stop_token stoken) {
    sockaddr_in client_addr;
    int client_socket;
    socklen_t clen = sizeof(client_addr);

    if (listen(server_sock, 1) < 0) {
      return;
    }

    while (true) {
      std::array<char, 256 * 2> buffer;
      client_socket = accept(server_sock, (struct sockaddr*)&client_addr, &clen);

      if (client_socket < 0) {
        continue;
      }

      int bytes_rec = read(client_socket, reinterpret_cast<char*>(&buffer), buffer.size());

      if (bytes_rec >= 0) {
        if (this->callback_) {
          if (bytes_rec > 0) {
            std::string client_ip = inet_ntoa(client_addr.sin_addr);
            int client_port = ntohs(client_addr.sin_port);
            auto res = this->callback_(
                client_ip, client_port,
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
    close(server_sock);
}

}  // namespace soc
}  // namespace com
}  // namespace srp
