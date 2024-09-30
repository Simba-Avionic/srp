/**
 * @file someip_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_CONTROLLER_SOMEIP_CONTROLLER_H_
#define ARA_COM_SOMEIP_CONTROLLER_SOMEIP_CONTROLLER_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "ara/com/i_com_client.h"
#include "ara/com/someip/controller/i_someip_controller.h"

namespace ara {
namespace com {
namespace someip {
class SomeipController : public IComClient, public ISomeipController {
 private:
  std::unordered_map<uint16_t, ISomeipController::ServiceCallback>
      service_list_;
  std::unordered_map<uint16_t, ISomeipController::ServiceCallback> proxy_list_;
  void SdHandler(const SomeipFrame& frame) noexcept;

 public:
  ara::core::Result<void> SendTo(
      uint32_t pid, const std::vector<uint8_t>& payload) noexcept override;
  ara::core::Result<void> SendByUdp(
      uint32_t port, const std::vector<uint8_t>& payload) noexcept override;
  void HandleNewMsg(uint32_t pid, const std::vector<uint8_t>& payload) noexcept;

  ara::core::Result<void> RegisterService(
      const uint16_t service_id, const uint16_t instance_id,
      ISomeipController::ServiceCallback&& callback) noexcept;

  ara::core::Result<void> RegisterProxy(const uint16_t service_id,
                                        const uint16_t instance_id,
                                        ServiceCallback&& callback) noexcept;

  static std::shared_ptr<SomeipController> GetInstance() noexcept;
  SomeipController(/* args */);
  ~SomeipController();
};

}  // namespace someip
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_CONTROLLER_SOMEIP_CONTROLLER_H_
