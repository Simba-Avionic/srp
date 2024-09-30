/**
 * @file udp_skeleton_bindings.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_BINDINGS_SKELETON_UDP_SKELETON_BINDINGS_H_
#define ARA_COM_SOMEIP_BINDINGS_SKELETON_UDP_SKELETON_BINDINGS_H_

#include <memory>
#include <vector>

#include "ara/com/someip/bindings/common_bindings.h"
#include "ara/com/someip/controller/i_someip_controller.h"

namespace ara {
namespace com {
namespace someip {
namespace bindings {
class UdpSkeletonBindings : public CommonBindings {
 private:
  const std::uint32_t port_;
  const std::shared_ptr<ISomeipController> controller_;

  ara::core::Result<void> RxCallback(
      uint32_t pid, const ara::com::someip::SomeipFrame& frame);

 public:
  explicit UdpSkeletonBindings(std::uint32_t port);
  void Start(std::stop_token token) override;
  void Stop() override;
  ara::core::Result<std::vector<uint8_t>> HandleMethod(
      const uint16_t& method_id, const std::vector<uint8_t>& payload) override;
  void HandleEvent(const uint16_t& method_id,
                   const std::vector<uint8_t>& payload) override;
  void SubscribeToEvent(const uint16_t& event_id) override;
};

}  // namespace bindings
}  // namespace someip
}  // namespace com
}  // namespace ara
#endif  // ARA_COM_SOMEIP_BINDINGS_SKELETON_UDP_SKELETON_BINDINGS_H_
