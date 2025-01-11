/**
 * @file proxy_connector.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/someip_demon/code/connector/proxy_connector.h"

#include <string>
#include <utility>
#include <vector>

#include "ara/com/someip/someip_frame_builder.h"
#include "ara/log/log.h"
#include "core/common/condition.h"

namespace srp {
namespace someip_demon {
namespace connectors {
namespace {
constexpr uint64_t kTimeOut{3000000000ul};
}  // namespace

ProxyConnector::ProxyConnector(const std::shared_ptr<IDb> db) : db_{db} {}

void ProxyConnector::Start() noexcept {
  time_out_thread_ = std::make_unique<std::jthread>(
      [this](std::stop_token token) { this->TimeOutLoop(token); });
}
void ProxyConnector::ProcessFrame(
    uint32_t pid, ara::com::someip::SomeipFrame&& frame) noexcept {
  std::optional<std::reference_wrapper<const db::FindServiceItem>> service_det;
  const uint64_t key{(pid << 16) + frame.header_.service_id};
  const auto& iter = cache_.find(key);
  if (iter == cache_.end()) {
    service_det = this->db_->GetServiceIp(frame.header_.service_id, pid);
    if (!service_det.has_value()) {
      ara::com::someip::SomeipFrameBuilder builder;
      builder.CopyFrameData(frame.header_)
          .SetMessageType(ara::com::someip::MessageType::kResponse)
          .SetReturnCode(ara::com::someip::MessageCode::kEUnknownService);
      auto raw = builder.BuildFrame().GetRaw();
      raw.insert(raw.begin(), 1U);
      this->ipc_soc_->TransmitToPid(pid, raw);
      return;
    }
    cache_.insert({key, service_det.value()});
  } else {
    service_det = iter->second;
  }
  const auto& service = service_det.value().get();
  this->req_list_.insert(
      {req_id,
       {frame.header_.request_id, frame.header_.service_id,
        frame.header_.method_id, pid, std::chrono::steady_clock::now()}});
  frame.header_.request_id = req_id;
  req_id++;
  this->udp_sock_->Transmit(inet_ntoa({ntohl(service.ip_)}), service.port_,
                            frame.GetRaw());
}

void ProxyConnector::ProcessFrame(
    const std::string& ip, std::uint16_t port,
    ara::com::someip::SomeipFrame&& frame) noexcept {
  const auto old_req = frame.header_.request_id;
  {
    const auto& iter = req_list_.find(frame.header_.request_id);
    if (iter == req_list_.end()) {
      return;
    }
    frame.header_.request_id = iter->second.ses_id;
    const auto raw = frame.GetRaw();
    std::vector<uint8_t> data{0x01};
    data.insert(data.end(), raw.begin(), raw.end());
    this->ipc_soc_->TransmitToPid(iter->second.process_id, std::move(data));
  }
  this->req_list_.erase(old_req);
}

void ProxyConnector::SendResult(const ReqDetails& det,
                                ara::com::someip::SomeipFrame frame) {
  const auto& iter = req_list_.find(frame.header_.request_id);
  if (iter == req_list_.end()) {
    return;
  }
  frame.header_.request_id = iter->second.ses_id;
  this->ipc_soc_->TransmitToPid(iter->second.process_id, frame.GetRaw());
  req_list_.erase(iter);
}

void ProxyConnector::TimeOutLoop(std::stop_token token) {
  while (!token.stop_requested()) {
    core::condition::wait_for(std::chrono::milliseconds{500}, token);
    std::vector<uint32_t> clear_list_{};
    for (const auto& item : this->req_list_) {
      // 3s
      if ((std::chrono::steady_clock::now() - item.second.time_point_).count() >
          kTimeOut) {
        ara::com::someip::SomeipFrameBuilder builder{};
        builder.SetServiceId(item.second.service_id)
            .SetMethodId(item.second.method_id)
            .SetSessionId(static_cast<uint16_t>(item.second.process_id))
            .SetRequestId(item.second.ses_id)
            .SetReturnCode(ara::com::someip::MessageCode::kETimeout)
            .SetMessageType(ara::com::someip::MessageType::kResponse);
        auto raw = builder.BuildFrame().GetRaw();
        raw.insert(raw.begin(), 1U);
        this->ipc_soc_->TransmitToPid(item.second.process_id, raw);
        clear_list_.push_back(item.first);
      }
    }
    for (const auto& key : clear_list_) {
      req_list_.erase(key);
    }
  }
}
}  // namespace connectors
}  // namespace someip_demon
}  // namespace srp
