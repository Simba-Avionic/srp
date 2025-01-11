/**
 * @file i_multicast_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_COM_I_MULTICAST_CONTROLLER_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_COM_I_MULTICAST_CONTROLLER_H_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "ara/core/result.h"
#include "platform/common/someip_demon/code/common/i_network_controller.h"
namespace srp {
namespace someip_demon {
namespace common {
namespace com {
class IMulticastController {
 public:
  virtual ~IMulticastController() = default;
  virtual ara::core::Result<void> Start() noexcept = 0;
  virtual void Stop() noexcept = 0;
  virtual ara::core::Result<void> SendFrame(const std::vector<uint8_t>& data,
                                            const std::string& ip,
                                            const uint16_t port) noexcept = 0;
  virtual void SetController(
      std::shared_ptr<INetworkController> controller) noexcept = 0;
};
}  // namespace com
}  // namespace common
}  // namespace someip_demon
}  // namespace srp

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_COM_I_MULTICAST_CONTROLLER_H_
