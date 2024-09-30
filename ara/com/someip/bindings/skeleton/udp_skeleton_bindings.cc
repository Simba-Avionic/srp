/**
 * @file udp_skeleton_bindings.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/com/someip/bindings/skeleton/udp_skeleton_bindings.h"

#include <vector>

#include "ara/com/log.h"
#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/someip/ServiceEntry.h"
#include "ara/com/someip/controller/someip_controller.h"
#include "ara/com/someip/someip_frame.h"
#include "ara/com/someip/someip_sd_frame_builder.h"

namespace ara {
namespace com {
namespace someip {
namespace bindings {

UdpSkeletonBindings::UdpSkeletonBindings(std::uint32_t port)
    : port_{port}, controller_{SomeipController::GetInstance()} {}

void UdpSkeletonBindings::Start(std::stop_token token) {
  controller_->RegisterService(
      this->service_id_, this->instance_id_,
      std::bind(&UdpSkeletonBindings::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2));
  // todo: Send offer to demon
  {
    SomeipSdFrameBuilder builder{};
    builder.AddOfferEntry(service_id_, instance_id_, major_version_,
                          minor_version_, 0U, port_);
    ara::com::LogInfo() << "Offer new Service: " << service_id_ << ", "
                        << instance_id_ << "on: 0.0.0.0:" << port_;
    this->controller_->SendByUdp(port_, builder.BuildFrame().GetRaw());
  }
}
ara::core::Result<void> UdpSkeletonBindings::RxCallback(
    uint32_t pid, const ara::com::someip::SomeipFrame& frame) {
  const auto& res =
      this->method_req_with_response_(frame.header_.method_id, frame.payload_);
  if (!res.HasValue()) {
    const auto res_f = SomeipFrame::MakeResponseFrameWithError(
        frame, static_cast<MessageCode>(res.Error().Value()));
    this->controller_->SendByUdp(port_, res_f.GetRaw());
    return {};
  }
  const auto res_f = SomeipFrame::MakeResponseFrame(frame, res.Value());
  this->controller_->SendByUdp(port_, res_f.GetRaw());
  return {};
}

void UdpSkeletonBindings::Stop() {}

ara::core::Result<std::vector<uint8_t>> UdpSkeletonBindings::HandleMethod(
    const uint16_t& method_id, const std::vector<uint8_t>& payload) {
  return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure,
                                 " Not supported");
}

void UdpSkeletonBindings::HandleEvent(const uint16_t& method_id,
                                      const std::vector<uint8_t>& payload) {}

void UdpSkeletonBindings::SubscribeToEvent(const uint16_t& event_id) {}

}  // namespace bindings
}  // namespace someip
}  // namespace com
}  // namespace ara
