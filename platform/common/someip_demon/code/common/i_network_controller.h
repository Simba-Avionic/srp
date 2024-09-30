#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_I_CONTROLLER_MANAGER_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_I_CONTROLLER_MANAGER_H_
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "platform/common/someip_demon/code/common/i_controller.h"

namespace simba {
namespace someip_demon {
namespace common {
class INetworkController {
 public:
  enum ControllerType : uint8_t {
    kSomeip = 0,
    kSomeipSd,
  };

 public:
  virtual void ProcessRawFrame(
      const std::string& ip, const std::uint16_t& port,
      const std::vector<std::uint8_t>& data) noexcept = 0;
  virtual void SetController(
      std::shared_ptr<IController> controller, ControllerType type) noexcept = 0;
  virtual ~INetworkController() = default;
};

}  // namespace common
}  // namespace someip_demon
}  // namespace simba

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_I_CONTROLLER_MANAGER_H_
