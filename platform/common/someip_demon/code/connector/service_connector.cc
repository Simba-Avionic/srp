/**
 * @file service_connector.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/someip_demon/code/connector/service_connector.h"

#include <arpa/inet.h>

#include <array>
#include <utility>

#include "ara/com/someip/message_type.h"
#include "ara/log/logging_menager.h"
#include "communication-core/sockets/stream_ipc_socket.h"
#include "core/common/condition.h"
#include "platform/common/someip_demon/code/connector/proxy_connector.h"
#include "platform/common/someip_demon/code/connector/sd_connector.h"
#include "platform/common/someip_demon/code/connector/skeleton_connector.h"

namespace simba {
namespace someip_demon {

namespace {
constexpr std::array<ara::com::someip::MessageType, 2> kSkeletonUdpType{
    ara::com::someip::MessageType::kRequest,
    ara::com::someip::MessageType::kRequestNoReturn};
constexpr std::array<ara::com::someip::MessageType, 3> kSkeletonIpcType{
    ara::com::someip::MessageType::kResponse,
    ara::com::someip::MessageType::kRequestAck,
    ara::com::someip::MessageType::kRequestNoReturnAck};
//  ---------------------------- PROXY --------------------------------------
constexpr std::array<ara::com::someip::MessageType, 2> kProxyIpcType{
    ara::com::someip::MessageType::kRequest,
    ara::com::someip::MessageType::kRequestNoReturn};
constexpr std::array<ara::com::someip::MessageType, 3> kProxyUdpType{
    ara::com::someip::MessageType::kResponse,
    ara::com::someip::MessageType::kRequestAck,
    ara::com::someip::MessageType::kRequestNoReturnAck};

template <typename T>
constexpr inline bool CheckMessageType(const T& container, uint8_t type) {
  return std::any_of(
      container.cbegin(), container.cend(),
      [type](const ara::com::someip::MessageType& x) {
        return static_cast<ara::com::someip::MessageType>(type) == x;
      });
}
}  // namespace

ServiceConnector::ServiceConnector(const std::string& ip, const uint16_t port,
                                   db::Database& sd_db)
    : ip_{ip},
      port_{port},
      logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger("scon")} {
  in_addr addr;
  inet_aton(ip_.c_str(), &addr);
  local_ip_ = htonl(addr.s_addr);
  auto temp_db =
      std::make_shared<connectors::SDConnector>(sd_db, local_ip_, port_);
  this->skeleton_connector_ =
      std::make_shared<connectors::SkeletonConnector>(temp_db);
  this->proxy_connector_ =
      std::make_shared<connectors::ProxyConnector>(temp_db);

  this->sd_connector_ = std::move(temp_db);
}

void ServiceConnector::Init() noexcept {
  logger_.LogInfo() << "Service controler: " << ip_ << ":" << port_
                    << " starting";
  if (udp_sock_ == nullptr) {
    udp_sock_ = std::make_shared<simba::common::soc::UdpSocket>();
    udp_sock_->SetRXCallback(
        std::bind(&ServiceConnector::UdpRxCallback, this, std::placeholders::_1,
                  std::placeholders::_2, ::std::placeholders::_3));
    const auto res = udp_sock_->Init(ip_, port_);
    if (res == simba::core::ErrorCode::kOk) {
      udp_sock_->StartRXThread();
      logger_.LogInfo() << "Service controler - UDP soc Started";
    } else {
      logger_.LogError() << "Service controler - UDP soc error: "
                         << static_cast<uint8_t>(res);
    }
  }
  if (ipc_soc_ == nullptr) {
    ipc_soc_ =
        std::make_shared<ara::com::ProccessSocket>(std::to_string(port_));

    ipc_soc_->SetCallback(std::bind(&ServiceConnector::IpcRxCallback, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2));
    ipc_soc_->Offer();
  }
  skeleton_connector_->RegisterSockets(ipc_soc_, udp_sock_);
  sd_connector_->RegisterSockets(ipc_soc_, udp_sock_);
  proxy_connector_->RegisterSockets(ipc_soc_, udp_sock_);

  skeleton_connector_->Start();
  sd_connector_->Start();
  proxy_connector_->Start();
}

void ServiceConnector::DeInit() noexcept {
  logger_.LogInfo() << "Service controller: " << ip_ << ":" << port_
                    << " stopping";
  udp_sock_.reset();
  ipc_soc_->StopOffer();
  ipc_soc_.reset();
}

void ServiceConnector::UdpRxCallback(const std::string& ip,
                                     const std::uint16_t& port,
                                     const std::vector<std::uint8_t>& payload) {
  logger_.LogError() << "[" << ip_ << ":" << port_ << "] New msg from: " << ip
                     << ":" << port << " with payload size: "
                     << static_cast<uint16_t>(payload.size());
  const auto frame_r = ara::com::someip::SomeipFrame::MakeFrame(payload);
  if (!frame_r.HasValue()) {
    logger_.LogError() << "[" << ip_ << ":" << port_
                       << "] Invalid frame from: " << ip << ":" << port;
    return;
  }
  auto frame = frame_r.Value();
  if ((frame.header_.service_id == 0xffff) &&
      (frame.header_.method_id == 0x8100)) {
    logger_.LogInfo() << "[" << ip_ << ":" << port_ << "] New Sd msg";
    // todo: support for sd
    return;
  } else {
    if (CheckMessageType(kSkeletonUdpType, frame.header_.message_type)) {
      skeleton_connector_->ProcessFrame(ip, port, std::move(frame));
    } else if (CheckMessageType(kProxyUdpType, frame.header_.message_type)) {
      this->proxy_connector_->ProcessFrame(ip, port, std::move(frame));
    }
  }
}

void ServiceConnector::IpcRxCallback(const uint32_t pid,
                                     const std::vector<std::uint8_t>& payload) {
  logger_.LogDebug() << "[" << ip_ << ":" << port_ << "] New msg from: " << pid
                     << " wit payload size: "
                     << static_cast<uint16_t>(payload.size());
  const auto frame_r = ara::com::someip::SomeipFrame::MakeFrame(
      std::vector<uint8_t>{payload.begin() + 1, payload.end()});
  if (!frame_r.HasValue()) {
    logger_.LogError() << "[" << ip_ << ":" << port_
                       << "] Invalid frame from: " << pid;
    return;
  }
  auto frame = frame_r.Value();

  if ((frame.header_.service_id == 0xffff) &&
      (frame.header_.method_id == 0x8100)) {
    logger_.LogInfo() << "[" << ip_ << ":" << port_ << "] New Sd msg -> "
                      << payload;
    sd_connector_->ProcessFrame(pid, std::move(frame));
    return;
  } else {
    if (CheckMessageType(kSkeletonIpcType, frame.header_.message_type)) {
      this->skeleton_connector_->ProcessFrame(pid, std::move(frame));
      return;
    } else if (CheckMessageType(kProxyIpcType, frame.header_.message_type)) {
      this->proxy_connector_->ProcessFrame(pid, std::move(frame));
      return;
    }
  }

  logger_.LogError() << "[" << ip_ << ":" << port_ << "] (ipc) New Not used ("
                     << frame.header_.service_id << "->"
                     << frame.header_.method_id << ") ";
}

ServiceConnector::~ServiceConnector() {}

}  // namespace someip_demon
}  // namespace simba
