/**
 * @file ipc_bindings.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/com/someip/bindings/ipc_bindings.h"

#include <algorithm>
#include <array>

#include "ara/com/log.h"
#include "ara/com/someip/message_code.h"
#include "ara/com/someip/message_type.h"

namespace ara {
namespace com {
namespace someip {
namespace bindings {
IPCSkeletonBindings::IPCSkeletonBindings(const ara::core::StringView path)
    : main_path_{path},
      buffor_{ara::core::InstanceSpecifier{"/" + path + "-buffor"}},
      cv_{ara::core::InstanceSpecifier{"/" + path + "-cv"}} {}
void IPCSkeletonBindings::Start(std::stop_token token) {
  ara::com::LogInfo() << "IPCSkeletonBindings: " << "Started";
  buffor_.OfferService();
  cv_.Offer();
}
void IPCSkeletonBindings::Stop() {}

ara::core::Result<std::vector<uint8_t>> IPCSkeletonBindings::HandleMethod(
    const uint16_t& method_id, const std::vector<uint8_t>& payload) {
  return ara::com::MakeErrorCode(ara::com::ComErrc::kUnsetFailure,
                                 " Not supported");
}

void IPCSkeletonBindings::HandleEvent(const uint16_t& method_id,
                                      const std::vector<uint8_t>& payload) {
  ara::com::LogDebug() << "IPCSkeletonBindings: " << "SomeIp event send";
  HeaderStructure header_{this->service_id_,
                          method_id,
                          0U,
                          0U,
                          0U,
                          1U,
                          this->major_version_,
                          MessageType::kNotification,
                          MessageCode::kEOk};
  const auto frame = ara::com::someip::SomeipFrame::MakeFrame(header_, payload);
  const auto vec = frame.GetRaw();
  ara::com::LogDebug() << "IPCSkeletonBindings: " << "Vec size: "
                       << static_cast<uint8_t>(vec.size());
  std::copy(vec.begin(), vec.end(), buffor_.GetNewSamplesPointer() + 2);
  *buffor_.GetNewSamplesPointer() = (*buffor_.GetNewSamplesPointer()) + 1;
  *(buffor_.GetNewSamplesPointer() + 1) = static_cast<uint8_t>(vec.size());
  cv_.NotifyAll();
}
void IPCSkeletonBindings::SubscribeToEvent(const uint16_t& event_id) {}

/////////////////////////  PROXY  ////////////////////////////////////

void IPCProxyBindings::ShmLoop(std::stop_token token) {
  std::stop_callback callback{token, [this]() { this->cv_.NotifyAll(); }};
  while (!token.stop_requested()) {
    cv_.Wait();
    if (token.stop_requested()) {
      break;
    }
    if (*buffor_.GetNewSamplesPointer() != last_msg_id) {
      const auto p_size = *(buffor_.GetNewSamplesPointer() + 1);
      uint8_t* buff = buffor_.GetNewSamplesPointer();
      //   // std::memcpy(buff, buffor_.GetNewSamplesPointer() + 2, p_size);
      std::vector<uint8_t> msg_v{buff + 2, buff + 2 + p_size};
      //   // delete[] buff;
      const auto& frame_r = ara::com::someip::SomeipFrame::MakeFrame(msg_v);
      if (frame_r.HasValue()) {
        const auto& frame = frame_r.Value();
        ara::com::LogDebug() << "[IPCProxyBindings]: New Event Value";
        ara::com::LogDebug()
            << "[IPCProxyBindings]: Service_id: " << frame.header_.service_id;
        ara::com::LogDebug()
            << "[IPCProxyBindings]: Method_id: " << frame.header_.method_id;
        this->event_notification_callback_(frame.header_.method_id,
                                           frame.payload_);
      } else {
        ara::com::LogError() << "[IPCProxyBindings]: " << frame_r.Error();
        ara::com::LogError()
            << "[IPCProxyBindings]: ID: " << *buffor_.GetNewSamplesPointer();
        ara::com::LogError() << "[IPCProxyBindings]: Size: "
                             << *(buffor_.GetNewSamplesPointer() + 1);
      }
    }
  }
}

IPCProxyBindings::IPCProxyBindings(const ara::core::StringView path)
    : buffor_{ara::core::InstanceSpecifier{"/" + path + "-buffor"}},
      cv_{ara::core::InstanceSpecifier{"/" + path + "-cv"}} {}
void IPCProxyBindings::Start(std::stop_token token) {
  ara::com::LogInfo() << "IPCProxyBindings: " << "Started";

  while (!token.stop_requested()) {
    if (buffor_.FindService().HasValue()) {
      break;
    } else {
      simba::core::condition::wait_for(std::chrono::milliseconds{500}, token);
    }
  }
  while (!token.stop_requested()) {
    if (cv_.Find().HasValue()) {
      break;
    } else {
      simba::core::condition::wait_for(std::chrono::milliseconds{500}, token);
    }
  }
  if (!token.stop_requested()) {
    shm_loop_thread_ = std::make_unique<std::jthread>(
        [this](std::stop_token token) { this->ShmLoop(token); });
  }
}
void IPCProxyBindings::Stop() {
  // shm_loop_thread_->request_stop();
  // shm_loop_thread_->join();
  shm_loop_thread_.release();
}
ara::core::Result<std::vector<uint8_t>> IPCProxyBindings::HandleMethod(
    const uint16_t& method_id, const std::vector<uint8_t>& payload) {
  ara::com::LogDebug() << "IPCSkeletonBindings: " << "SomeIp Method Send";
  HeaderStructure header_{this->service_id_,
                          method_id,
                          0U,
                          0U,
                          0U,
                          1U,
                          this->major_version_,
                          MessageType::kRequest,
                          MessageCode::kEOk};
  const auto frame = ara::com::someip::SomeipFrame::MakeFrame(header_, payload);
  const auto vec = frame.GetRaw();
  // TODO(Bartosz Snieg): Send and Recive

  return std::vector<uint8_t>{};
}

void IPCProxyBindings::HandleEvent(const uint16_t& method_id,
                                   const std::vector<uint8_t>& payload) {}

void IPCProxyBindings::SubscribeToEvent(const uint16_t& event_id) {
  this->subscribe_new_status_(event_id, SubscribeStatus::kSubscribe);
}
}  // namespace bindings
}  // namespace someip
}  // namespace com
}  // namespace ara
