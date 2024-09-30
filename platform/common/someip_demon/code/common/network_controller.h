#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_NETWORK_CONTROLLER_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_NETWORK_CONTROLLER_H_

#include "platform/common/someip_demon/code/common/i_controller.h"
#include "platform/common/someip_demon/code/common/i_network_controller.h"
#include "ara/log/logger.h"
namespace simba {
namespace someip_demon {
namespace common {
class NetworkController : public INetworkController {
 private:
  const ara::log::Logger& logger_;
  std::shared_ptr<IController> sd_controller_;
 public:
  NetworkController(/* args */);
  ~NetworkController();
  void ProcessRawFrame(const std::string& ip, const std::uint16_t& port,
                       const std::vector<std::uint8_t>& data) noexcept override;
  void SetController(std::shared_ptr<IController> controller,
                     INetworkController::ControllerType type) noexcept override;
};

}  // namespace common
}  // namespace someip_demon
}  // namespace simba

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_NETWORK_CONTROLLER_H_
