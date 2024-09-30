/**
 * @file udp_proxy_bindings.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/com/someip/bindings/proxy/udp_proxy_bindings.h"

#include <unistd.h>

#include <vector>
#include <utility>

#include "ara/com/log.h"
#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/someip/ServiceEntry.h"
#include "ara/com/someip/controller/someip_controller.h"
#include "ara/com/someip/someip_frame_builder.h"
#include "ara/com/someip/someip_sd_frame_builder.h"
#include "ara/com/someip_error.h"

namespace ara {
namespace com {
namespace someip {
namespace bindings {

UdpProxyBindings::UdpProxyBindings(std::uint32_t port)
    : port_{port},
      controller_{SomeipController::GetInstance()},
      pid_{static_cast<uint16_t>(getpid())} {}

void UdpProxyBindings::Start(std::stop_token token) {
  controller_->RegisterProxy(
      this->service_id_, this->instance_id_,
      std::bind(&UdpProxyBindings::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2));

  SomeipSdFrameBuilder builder{};
  builder.AddFindEntry(service_id_, instance_id_, major_version_,
                       minor_version_);

  ara::com::LogInfo() << "Find new Service: " << service_id_ << ", "
                      << instance_id_ << "on: 0.0.0.0:" << port_;
  this->controller_->SendByUdp(port_, builder.BuildFrame().GetRaw());
  std::unique_lock lk(find_mutex_);
  this->find_cv_.wait(lk, [this] { return this->find_status_flag_; });
}

ara::core::Result<void> UdpProxyBindings::RxCallback(
    uint32_t pid, const ara::com::someip::SomeipFrame& frame) {
  if ((frame.header_.service_id == 0xffff) &&
      (frame.header_.method_id == 0x8100)) {
    ara::com::LogInfo() << "SD msg recived";
    find_status_flag_ = true;
    find_cv_.notify_all();
    return {};
  } else {
    if (frame.header_.service_id == service_id_) {
      if (frame.header_.method_id < 0x8000U) {
        const auto& iter = req_list_.find(frame.header_.request_id);
        if (iter == req_list_.end()) {
          return {};
        }

        iter->second->payload = frame.payload_;
        iter->second->status_ = frame.header_.return_code;
        iter->second->cv_.notify_all();
      } else {
        this->event_notification_callback_(frame.header_.method_id,
                                           frame.payload_);
        return {};
      }
    }
  }
  return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure,
                                 " Not supported");
}

void UdpProxyBindings::Stop() {}

ara::core::Result<std::vector<uint8_t>> UdpProxyBindings::HandleMethod(
    const uint16_t& method_id, const std::vector<uint8_t>& payload) {
  SomeipFrameBuilder builder{};
  builder.SetServiceId(service_id_).SetMethodId(method_id).SetSessionId(pid_);
  builder.SetRequestId(req_id);  // todo(snieg45@gmail.com) implement this
  builder.SetMessageType(MessageType::kRequest)
      .SetReturnCode(MessageCode::kEOk);
  builder.SetPayload(std::move(payload));
  this->controller_->SendByUdp(port_, builder.BuildFrame().GetRaw());
  auto t = std::make_shared<RequestTableItem>(req_id);
  req_list_.insert(std::make_pair(req_id, t));
  req_id++;
  std::unique_lock lk(t->block_mutex);
  t->cv_.wait(lk, [this, t] { return t->status_ != 0xFF; });
  const auto r_code = t->status_;
  if (t->status_ == MessageCode::kEOk) {
    req_list_.erase(t->req_id_);
    return t->payload;
  }
  req_list_.erase(t->req_id_);
  return ara::com::someip::MakeErrorCode(
      static_cast<ara::com::someip::MessageCode>(r_code), "Error in response");
}

void UdpProxyBindings::HandleEvent(const uint16_t& method_id,
                                   const std::vector<uint8_t>& payload) {}

void UdpProxyBindings::SubscribeToEvent(const uint16_t& event_id) {
  this->subscribe_new_status_(event_id, SubscribeStatus::kSubscribePending);
}

}  // namespace bindings
}  // namespace someip
}  // namespace com
}  // namespace ara
