/**
 * @file proccess_socket.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef BINDINGS_COMMON_SOCKET_PROCCESS_SOCKET_H_
#define BINDINGS_COMMON_SOCKET_PROCCESS_SOCKET_H_

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/core/result.h"

namespace ara {
namespace com {
class ProccessSocket {
 public:
  using RxCallback =
      std::function<void(const uint32_t, const std::vector<uint8_t>&)>;

 private:
  const uint32_t local_pid_;
  const std::string local_soc_;
  int sfd_;
  sockaddr_un addr_;
  RxCallback callback_;
  std::unique_ptr<std::jthread> rx_thread_;
  std::mutex sending_m_{};
  void RxLoop(std::stop_token token) noexcept;

 public:
  void SetCallback(RxCallback&& callback);
  ara::core::Result<void> Offer() noexcept;
  ara::core::Result<void> StopOffer() noexcept;
  ara::core::Result<void> TransmitToPid(
      uint16_t dest_pid, const std::vector<uint8_t>& payload) noexcept;
  ara::core::Result<void> Transmit(
      const std::string& path, const std::vector<uint8_t>& payload) noexcept;
  ProccessSocket(/* args */);
  explicit ProccessSocket(const uint32_t app_id);
  explicit ProccessSocket(const std::string& sock_path_);
  ~ProccessSocket();
};

}  // namespace com
}  // namespace ara

#endif  // BINDINGS_COMMON_SOCKET_PROCCESS_SOCKET_H_
