/**
 * @file i_network_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_I_NETWORK_CONTROLLER_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_I_NETWORK_CONTROLLER_H_
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "platform/common/someip_demon/code/common/i_controller.h"

namespace srp {
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
}  // namespace srp

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_I_NETWORK_CONTROLLER_H_
