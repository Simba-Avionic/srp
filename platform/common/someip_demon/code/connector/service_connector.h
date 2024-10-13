#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SERVICE_CONNECTOR_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SERVICE_CONNECTOR_H_

#include <cstdint>
#include <memory>
#include <string>

#include "ara/log/logger.h"
#include "communication-core/sockets/Isocket_stream.h"
#include "platform/common/someip_demon/code/common/udp_socket.h"

namespace simba {
namespace someip_demon {
class ServiceConnector {
 private:
  const std::string ip_;
  const std::uint16_t port_;
  const ara::log::Logger& logger_;
  std::unique_ptr<simba::common::soc::UdpSocket> udp_sock_;
  std::unique_ptr<simba::com::soc::ISocketStream> ipc_soc_;
  void UdpRxCallback(const std::string& ip, const std::uint16_t& port,
                     const std::vector<std::uint8_t>& payload);
  std::vector<uint8_t> IpcRxCallback(const std::string& ip,
                                     const std::uint16_t& port,
                                     std::vector<std::uint8_t> payload);

 public:
  ServiceConnector(const std::string& ip, const uint16_t port_);
  void Init() noexcept;
  void DeInit() noexcept;
  ~ServiceConnector();
};
}  // namespace someip_demon
}  // namespace simba

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SERVICE_CONNECTOR_H_
