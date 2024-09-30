/**
 * @file someip_handler.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_SOMEIP_HANDLER_H_
#define ARA_COM_SOMEIP_SOMEIP_HANDLER_H_
#include <cstdint>
#include <memory>
#include <vector>

#include "ara/com/someip/bindings/i_bindings.h"
#include "ara/com/someip/someip_frame.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace someip {
class ServiceHandler {
 protected:
  const ara::core::InstanceSpecifier instance_;
  const std::uint16_t service_id_;
  bool founded_{false};

  std::uint16_t instance_id_{0U};
  std::uint8_t major_version_{0U};
  std::uint32_t minor_version_{0U};

  std::shared_ptr<bindings::IBindings> bindings_{nullptr};

  virtual void HandleEvent(const uint16_t event_id,
                           const std::vector<uint8_t>& data) = 0;
  virtual void SubscribeNewStatus(const uint16_t event_id_,
                                  const uint8_t new_status) = 0;
  ara::core::Result<std::vector<uint8_t>> HandleMethod(
      const uint16_t& method_id, const std::vector<uint8_t>& data) {
    return this->bindings_->HandleMethod(method_id, data);
  }

 public:
  void SubscribeEvent(const uint16_t& event_id_);
  ara::core::Result<void> StartFind(std::stop_token token);
  ara::core::Result<void> StopFind();
  ServiceHandler(const ara::core::InstanceSpecifier& instance,
                 uint16_t service_id);
  ~ServiceHandler() = default;
};

}  // namespace someip
}  // namespace com
}  // namespace ara
#endif  // ARA_COM_SOMEIP_SOMEIP_HANDLER_H_
