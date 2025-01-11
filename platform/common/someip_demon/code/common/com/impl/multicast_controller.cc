/**
 * @file multicast_controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/someip_demon/code/common/com/impl/multicast_controller.h"

#include <future> // NOLINT
#include <memory>
#include <vector>

#include "ara/log/logging_menager.h"

namespace srp {
namespace someip_demon {
namespace common {
namespace com {
MulticastController::MulticastController(const std::string& interface_ip,
                                         const std::string& multicast_ip,
                                         const uint16_t multicast_port)
    : interface_ip_{interface_ip},
      multicast_ip_{multicast_ip},
      multicast_port_{multicast_port},
      logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "mUDP", "", ara::log::LogLevel::kError)} {}

void MulticastController::RxCallback(const std::string& ip,
                                     const std::uint16_t& port,
                                     const std::vector<std::uint8_t>& data) {
  if (data.size() < 0x10) {
    return;
  }
  if (controller_ != nullptr) {
    std::async(std::launch::async, [this, &ip, &port, &data]() {
      controller_->ProcessRawFrame(ip, port, data);
    });
  }
}

MulticastController::~MulticastController() {}

ara::core::Result<void> MulticastController::Start() noexcept {
  sock_ = std::make_unique<srp::common::soc::UdpMulticastSocket>();
  if (sock_->Init(interface_ip_, multicast_ip_, multicast_port_) ==
      core::ErrorCode::kOk) {
    sock_->SetRXCallback(std::bind(&MulticastController::RxCallback, this,
                                   std::placeholders::_1, std::placeholders::_2,
                                   std::placeholders::_3));
    sock_->StartRXThread();
    logger_.LogInfo() << "Multicas socket start on: " << interface_ip_;
    logger_.LogInfo() << "Join to multicast group: " << multicast_ip_ << ":"
                      << multicast_port_;
  }
  return {};
}
void MulticastController::Stop() noexcept { sock_.release(); }

ara::core::Result<void> MulticastController::SendFrame(
    const std::vector<uint8_t>& data, const std::string& ip,
    const uint16_t port) noexcept {
  logger_.LogInfo() << "Transmit msg to: " << ip << ":" << port;
  if (sock_ != nullptr) {
    this->sock_->Transmit(data);
  }
  return {};
}
void MulticastController::SetController(
    std::shared_ptr<INetworkController> controller) noexcept {
  controller_ = controller;
}
}  // namespace com
}  // namespace common
}  // namespace someip_demon
}  // namespace srp
