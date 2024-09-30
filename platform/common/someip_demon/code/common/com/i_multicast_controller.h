#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_COM_I_MULTICAST_CONTROLLER_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_COM_I_MULTICAST_CONTROLLER_H_

#include <cstdint>
#include <string>
#include <vector>

#include "ara/core/result.h"
#include "platform/common/someip_demon/code/common/i_network_controller.h"
namespace simba {
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
  virtual void SetController(std::shared_ptr<INetworkController> controller) noexcept = 0;
};
}  // namespace com
}  // namespace common
}  // namespace someip_demon
}  // namespace simba

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_COM_I_MULTICAST_CONTROLLER_H_
