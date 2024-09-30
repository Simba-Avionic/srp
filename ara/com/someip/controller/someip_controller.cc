/**
 * @file someip_controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/com/someip/controller/someip_controller.h"

#include <utility>
#include <vector>

#include "ara/com/log.h"
#include "ara/com/someip/EndpointOption.h"
#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/someip/ServiceEntry.h"
#include "ara/com/someip/message_type.h"
#include "ara/com/someip/someip_frame.h"

namespace ara {
namespace com {
namespace someip {
namespace {
constexpr std::array<ara::com::someip::MessageType, 2> kSkeletonUdpType{
    ara::com::someip::MessageType::kRequest,
    ara::com::someip::MessageType::kRequestNoReturn};
constexpr std::array<ara::com::someip::MessageType, 3> kSkeletonIpcType{
    ara::com::someip::MessageType::kResponse,
    ara::com::someip::MessageType::kRequestAck,
    ara::com::someip::MessageType::kRequestNoReturnAck};
//  ---------------------------- PROXY --------------------------------------
constexpr std::array<ara::com::someip::MessageType, 2> kProxyIpcType{
    ara::com::someip::MessageType::kRequest,
    ara::com::someip::MessageType::kRequestNoReturn};
constexpr std::array<ara::com::someip::MessageType, 3> kProxyUdpType{
    ara::com::someip::MessageType::kResponse,
    ara::com::someip::MessageType::kRequestAck,
    ara::com::someip::MessageType::kRequestNoReturnAck};

template <typename T>
constexpr inline bool CheckMessageType(const T& container, uint8_t type) {
  return std::any_of(
      container.cbegin(), container.cend(),
      [type](const ara::com::someip::MessageType& x) {
        return static_cast<ara::com::someip::MessageType>(type) == x;
      });
}

std::shared_ptr<SomeipController> instance_;
}  // namespace

SomeipController::SomeipController(/* args */) {}

SomeipController::~SomeipController() {}

void SomeipController::HandleNewMsg(
    uint32_t pid, const std::vector<uint8_t>& payload) noexcept {
  ara::com::LogDebug() << "[Someip] New msg from: " << pid;
  const auto frame_r = SomeipFrame::MakeFrame(payload);
  if (!frame_r.HasValue()) {
    return;
  }
  const auto& frame = frame_r.Value();
  if ((frame.header_.service_id == 0xffff) &&
      (frame.header_.method_id == 0x8100)) {
    ara::com::LogError() << "[Someip] New SD msg";
    this->SdHandler(frame);
    return;
  }
  if (CheckMessageType(kSkeletonUdpType, frame.header_.message_type)) {
    const auto& item = service_list_.find(frame.header_.service_id);
    if (item == service_list_.end()) {
      ara::com::LogError() << "Service: " << frame.header_.service_id
                           << " not exist!";
      // TODO(Bartosz Snieg): send Error !
      return;
    }
    item->second(pid, std::move(frame));
  } else if (CheckMessageType(kProxyUdpType, frame.header_.message_type)) {
    const auto& item = proxy_list_.find(frame.header_.service_id);
    if (item == proxy_list_.end()) {
      ara::com::LogError() << "Service: " << frame.header_.service_id
                           << " not exist!";
      // TODO(Bartosz Snieg): send Error !
      return;
    }
    item->second(pid, std::move(frame));
  }
}

std::shared_ptr<SomeipController> SomeipController::GetInstance() noexcept {
  if (instance_ == nullptr) {
    instance_ = std::make_shared<SomeipController>();
  }
  return instance_;
}
ara::core::Result<void> SomeipController::RegisterService(
    const uint16_t service_id, const uint16_t instance_id,
    ISomeipController::ServiceCallback&& callback) noexcept {
  if (service_list_.insert({service_id, callback}).second) {
    return {};
  } else {
    return MakeErrorCode(ComErrc::kCommunicationLinkError,
                         " Service already offered! ");
  }
}

ara::core::Result<void> SomeipController::RegisterProxy(
    const uint16_t service_id, const uint16_t instance_id,
    ServiceCallback&& callback) noexcept {
  if (proxy_list_.insert({service_id, callback}).second) {
    return {};
  } else {
    return MakeErrorCode(ComErrc::kCommunicationLinkError,
                         " Proxy already exist! ");
  }
}

void SomeipController::SdHandler(const SomeipFrame& frame) noexcept {
  const auto raw =
      std::vector<uint8_t>(frame.Payload().begin() + 4, frame.Payload().end());
  auto count = (ara::com::Convert<uint32_t>::Conv(raw).value_or(0U));
  if constexpr (std::endian::native != std::endian::big) {
    count = ara::com::EndianConvert<std::uint32_t>::Conv(count) / 16;
  }
  ara::com::LogInfo() << "[Someip] count: " << count;
  for (auto i = 0; i < count; i++) {
    if (*(raw.begin() + 4 + (16 * i)) == 0x01) {
      const auto t = ara::com::Convert<ara::com::someip::ServiceEntry>::Conv(
          std::vector<uint8_t>{raw.begin() + 4 + (16 * i), raw.end()});
      if (t.HasValue()) {
        const auto& iter = this->proxy_list_.find(t.Value().service_id);
        if (iter != this->proxy_list_.end()) {
          iter->second(0, frame);
        }
      }
    }
  }
}

ara::core::Result<void> SomeipController::SendTo(
    uint32_t pid, const std::vector<uint8_t>& payload) noexcept {}

ara::core::Result<void> SomeipController::SendByUdp(
    uint32_t port, const std::vector<uint8_t>& payload) noexcept {
  ara::com::LogDebug() << "[Someip] Sending on: " << port;
  if (send_callback_to_) {
    this->send_callback_to_(std::to_string(port), std::move(payload),
                            IComClient::MsgType::kSomeIp);
  }
  return {};
}
}  // namespace someip
}  // namespace com
}  // namespace ara
