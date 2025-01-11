/**
 * @file service_connector.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SERVICE_CONNECTOR_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SERVICE_CONNECTOR_H_

#include <chrono>  // NOLINT
#include <cstdint>
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <unordered_map>
#include <vector>

#include "bindings/common/socket/proccess_socket.h"
#include "ara/com/someip/ServiceEntry.h"
#include "ara/com/someip/someip_frame.h"
#include "ara/log/logger.h"
#include "platform/common/someip_demon/code/common/udp_socket.h"
#include "platform/common/someip_demon/code/connector/i_connector.h"
#include "platform/common/someip_demon/code/db/database.h"

namespace srp {
namespace someip_demon {
class ServiceConnector {
 private:
  const std::string ip_;
  uint32_t local_ip_{0U};
  const std::uint16_t port_;
  const ara::log::Logger& logger_;
  std::shared_ptr<srp::common::soc::UdpSocket> udp_sock_;
  std::shared_ptr<ara::com::ProccessSocket> ipc_soc_;

  std::shared_ptr<connectors::IConnector> sd_connector_;
  std::shared_ptr<connectors::IConnector> skeleton_connector_;
  std::shared_ptr<connectors::IConnector> proxy_connector_;

  void UdpRxCallback(const std::string& ip, const std::uint16_t& port,
                     const std::vector<std::uint8_t>& payload);
  void IpcRxCallback(const uint32_t pid,
                     const std::vector<std::uint8_t>& payload);

 public:
  ServiceConnector(const std::string& ip, const uint16_t port_,
                   db::Database& sd_db);  // NOLINT
  void Init() noexcept;
  void DeInit() noexcept;
  ~ServiceConnector();
};
}  // namespace someip_demon
}  // namespace srp

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SERVICE_CONNECTOR_H_
