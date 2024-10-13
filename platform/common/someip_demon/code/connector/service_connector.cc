
#include "platform/common/someip_demon/code/connector/service_connector.h"

#include "ara/log/logging_menager.h"
#include "communication-core/sockets/stream_ipc_socket.h"

namespace simba {
namespace someip_demon {
ServiceConnector::ServiceConnector(const std::string& ip, const uint16_t port)
    : ip_{ip},
      port_{port},
      logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger("scon")} {}

void ServiceConnector::Init() noexcept {
  logger_.LogInfo() << "Service controler: " << ip_ << ":" << port_
                    << " starting";
  if (udp_sock_ == nullptr) {
    udp_sock_ = std::make_unique<simba::common::soc::UdpSocket>();
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
    if(ipc_soc_ == nullptr) {
        ipc_soc_ = std::make_unique<simba::com::soc::StreamIpcSocket>();
        ipc_soc_->SetRXCallback(std::bind(&ServiceConnector::IpcRxCallback, this, std::placeholders::_1,
                  std::placeholders::_2, ::std::placeholders::_3));
        ipc_soc_->Init(simba::com::soc::SocketConfig{std::to_string(port_),0,0});
        ipc_soc_->StartRXThread();
    }

}
void ServiceConnector::DeInit() noexcept {
  logger_.LogInfo() << "Service controler: " << ip_ << ":" << port_
                    << " stopping";
  udp_sock_.release();
  ipc_soc_.release();
}
void ServiceConnector::UdpRxCallback(const std::string& ip,
                                     const std::uint16_t& port,
                                     const std::vector<std::uint8_t>& payload) {
  logger_.LogInfo() << "[" << ip_ << ":" << port_ << "] New msg from: " << ip
                    << ":" << port << " with payload size: "
                    << static_cast<uint16_t>(payload.size());
}

std::vector<uint8_t> ServiceConnector::IpcRxCallback(const std::string& ip,
                                     const std::uint16_t& port,
                                     std::vector<std::uint8_t> payload) {
                                        return {};
                                     }

ServiceConnector::~ServiceConnector() {}

}  // namespace someip_demon
}  // namespace simba