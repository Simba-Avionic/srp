/**
 * @file i_someip_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_CONTROLLER_I_SOMEIP_CONTROLLER_H_
#define ARA_COM_SOMEIP_CONTROLLER_I_SOMEIP_CONTROLLER_H_
#include <functional>
#include <memory>
#include <vector>

#include "ara/com/someip/someip_frame.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace someip {

class ISomeipController {
 public:
  using ServiceCallback = std::function<ara::core::Result<void>(
      uint32_t pid, const ara::com::someip::SomeipFrame& frame)>;

  virtual ara::core::Result<void> SendTo(
      uint32_t pid, const std::vector<uint8_t>& payload) noexcept = 0;
  virtual ara::core::Result<void> SendByUdp(
      uint32_t port, const std::vector<uint8_t>& payload) noexcept = 0;
  virtual ~ISomeipController() = default;

  virtual ara::core::Result<void> RegisterService(
      const uint16_t service_id, const uint16_t instance_id,
      ServiceCallback&& callback) noexcept = 0;
  virtual ara::core::Result<void> RegisterProxy(
      const uint16_t service_id, const uint16_t instance_id,
      ServiceCallback&& callback) noexcept = 0;
};
}  // namespace someip
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_CONTROLLER_I_SOMEIP_CONTROLLER_H_
