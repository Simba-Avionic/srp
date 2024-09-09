/**
 * @file service_skeleton.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_SERVICE_SKELETON_H_
#define ARA_COM_SOMEIP_SERVICE_SKELETON_H_

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
class ServiceSkeleton {
 protected:
  const ara::core::InstanceSpecifier instance_;
  const std::uint16_t service_id_;
  bool offered_{false};

  std::shared_ptr<bindings::IBindings> bindings_{nullptr};

  virtual ara::core::Result<std::vector<uint8_t>> HandleMethod(
      const uint16_t method_id, const std::vector<uint8_t>& data) = 0;

  void HandleEvent(const uint16_t event_id, const std::vector<uint8_t>& data);

  virtual void BinderReady() {}

  /**
   * @brief This method will be call whene we recive call method without
   * response / Not used now
   *
   * @param method_id method id
   * @param payload req payload vector
   */
  void Handler(const uint16_t& method_id, const std::vector<uint8_t>& payload);

  /**
   * @brief This method will be call whene we recive call method with response
   *
   * @param method_id
   * @param payload
   * @return ara::core::Result<std::vector<uint8_t>> vector with value or error
   */
  ara::core::Result<std::vector<uint8_t>> HandlerWithResult(
      const uint16_t& method_id, const std::vector<uint8_t>& payload);

 public:
  ara::core::Result<void> StartOffer();
  ara::core::Result<void> StopOffer();
  ServiceSkeleton(const ara::core::InstanceSpecifier& instance,
                  uint16_t service_id);
  ~ServiceSkeleton();
};

}  // namespace someip
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_SERVICE_SKELETON_H_
