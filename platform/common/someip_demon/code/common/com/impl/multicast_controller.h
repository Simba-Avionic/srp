/**
 * @file multicast_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_COM_IMPL_MULTICAST_CONTROLLER_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_COM_IMPL_MULTICAST_CONTROLLER_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "ara/log/logger.h"
#include "platform/common/someip_demon/code/common/com/i_multicast_controller.h"
#include "platform/common/someip_demon/code/common/udp_multicast_socket.h"
namespace srp {
namespace someip_demon {
namespace common {
namespace com {
class MulticastController : public IMulticastController {
 private:
  const std::string interface_ip_;
  const std::string multicast_ip_;
  const uint16_t multicast_port_;
  const ara::log::Logger logger_;
  std::unique_ptr<srp::common::soc::UdpMulticastSocket> sock_;
  std::shared_ptr<INetworkController> controller_;
  void RxCallback(const std::string& ip, const std::uint16_t& port,
                  const std::vector<std::uint8_t>& data);

 public:
  MulticastController(const std::string& interface_ip,
                      const std::string& multicast_ip,
                      const uint16_t multicast_port);
  ~MulticastController();
  ara::core::Result<void> Start() noexcept override;
  void Stop() noexcept override;
  ara::core::Result<void> SendFrame(const std::vector<uint8_t>& data,
                                    const std::string& ip,
                                    const uint16_t port) noexcept override;
  void SetController(
      std::shared_ptr<INetworkController> controller) noexcept override;
};
}  // namespace com
}  // namespace common
}  // namespace someip_demon
}  // namespace srp

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_COM_IMPL_MULTICAST_CONTROLLER_H_
