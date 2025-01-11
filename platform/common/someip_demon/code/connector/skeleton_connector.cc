/**
 * @file skeleton_connector.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "platform/common/someip_demon/code/connector/skeleton_connector.h"

#include <string>
#include <utility>
#include <vector>

#include "core/common/condition.h"

namespace srp {
namespace someip_demon {
namespace connectors {
namespace {
constexpr uint64_t kTimeOut{3000000000ul};
}  // namespace

SkeletonConnector::SkeletonConnector(const std::shared_ptr<IDb> db) : db_{db} {}

void SkeletonConnector::Start() noexcept {
  time_out_thread_ = std::make_unique<std::jthread>(
      [this](std::stop_token token) { this->TimeOutLoop(token); });
}
void SkeletonConnector::ProcessFrame(
    uint32_t pid, ara::com::someip::SomeipFrame&& frame) noexcept {
  const auto& item = req_list_.find(frame.header_.session_id);
  if (item != req_list_.end()) {
    SendResult(item->second, frame);
    req_list_.erase(item->first);
    return;
  }
}

void SkeletonConnector::ProcessFrame(
    const std::string& ip, std::uint16_t port,
    ara::com::someip::SomeipFrame&& frame) noexcept {
  const auto item = db_->GetServiceProviderPid(frame.header_.service_id);
  if (!item.has_value()) {
    const auto res = ara::com::someip::SomeipFrame::MakeResponseFrameWithError(
        frame, ara::com::someip::MessageCode::kEUnknownService);
    this->udp_sock_->Transmit(ip, port, res.GetRaw());
    return;
  }
  this->req_list_.insert(
      {req_id,
       {ip, port, frame.header_.session_id, std::chrono::steady_clock::now()}});
  frame.header_.session_id = req_id;
  req_id++;
  const auto raw = frame.GetRaw();
  std::vector<uint8_t> data{0x01};
  data.insert(data.end(), raw.begin(), raw.end());
  this->ipc_soc_->TransmitToPid(item.value(), std::move(data));
}

void SkeletonConnector::SendResult(const ReqDetails& det,
                                   ara::com::someip::SomeipFrame frame) {
  frame.header_.session_id = det.ses_id;
  this->udp_sock_->Transmit(det.ip_, det.port_, frame.GetRaw());
}

void SkeletonConnector::TimeOutLoop(std::stop_token token) {
  while (!token.stop_requested()) {
    core::condition::wait_for(std::chrono::milliseconds{500}, token);
    std::vector<uint32_t> clear_list_{};
    for (const auto& item : this->req_list_) {
      // 3s
      if ((std::chrono::steady_clock::now() - item.second.time_point_).count() >
          kTimeOut) {
        // logger_.LogError() << "[" << ip_ << ":" << port_
        //                    << "] Time out for req_id: " << item.first
        //                    << " from: " << item.second.ip_ << ":"
        //                    << item.second.port_;

        // todo send timeout

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
