/**
 * @file i_connector.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_I_CONNECTOR_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_I_CONNECTOR_H_
#include <memory>
#include <string>

#include "bindings/common/socket/proccess_socket.h"
#include "ara/com/someip/someip_frame.h"
#include "platform/common/someip_demon/code/common/udp_socket.h"
namespace srp {
namespace someip_demon {
namespace connectors {
class IConnector {
 public:
  virtual void RegisterSockets(
      std::shared_ptr<ara::com::ProccessSocket> ipc_soc,
      std::shared_ptr<srp::common::soc::UdpSocket> udp_sock) = 0;
  virtual void ProcessFrame(uint32_t pid,
                            ara::com::someip::SomeipFrame&& frame) noexcept = 0;
  virtual void ProcessFrame(const std::string& ip, std::uint16_t port,
                            ara::com::someip::SomeipFrame&& frame) noexcept = 0;
  virtual void Start() noexcept = 0;
  virtual ~IConnector() = default;
};
}  // namespace connectors
}  // namespace someip_demon
}  // namespace srp
#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_I_CONNECTOR_H_
