/**
 * @file sd_connector.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "platform/common/someip_demon/code/connector/sd_connector.h"

#include "ara/com/someip/EndpointOption.h"
#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/someip/someip_sd_frame_builder.h"
#include "ara/log/log.h"

namespace srp {
namespace someip_demon {
namespace connectors {

SDConnector::SDConnector(db::Database& sd_db, std::uint32_t local_ip,
                         std::uint16_t port)
    : local_ip_{local_ip}, port_{port}, sd_db_{sd_db} {}

void SDConnector::ProcessFrame(uint32_t pid,
                               ara::com::someip::SomeipFrame&& frame) noexcept {
  SdMsgHandler(pid, frame);
}
void SDConnector::ProcessFrame(const std::string& ip, std::uint16_t port,
                               ara::com::someip::SomeipFrame&& frame) noexcept {
}
void SDConnector::Start() noexcept {
  this->sd_thread_ = std::make_unique<std::jthread>(
      std::bind_front(&SDConnector::SdLoop, this));
}
std::optional<uint32_t> SDConnector::GetServiceProviderPid(
    const uint16_t service_id) const noexcept {
  const auto& iter = this->provider_service_list_.find(service_id);
  if (iter == this->provider_service_list_.cend()) {
    return std::nullopt;
  }
  return iter->second;
}

std::optional<std::reference_wrapper<const db::FindServiceItem>>
SDConnector::GetServiceIp(const uint16_t service_id,
                          const uint32_t pid) const noexcept {
  const auto& iter1 = this->consume_service_list_.find(pid);
  if (iter1 == this->consume_service_list_.end()) {
    return std::nullopt;
  }
  const auto& iter2 = iter1->second.find(service_id);
  if (iter2 == iter1->second.end()) {
    return std::nullopt;
  }
  if (!iter2->second.provided_) {
    return std::nullopt;
  }
  return iter2->second.sd_object_;
}

void SDConnector::SdMsgHandler(const uint32_t pid,
                               const ara::com::someip::SomeipFrame& frame) {
  const auto& raw = frame.payload_;
  auto count = (srp::data::Convert<uint32_t>::Conv(
                    std::vector<uint8_t>{raw.begin() + 4, raw.end()})
                    .value_or(0U));
  if constexpr (std::endian::native != std::endian::big) {
    count = srp::data::EndianConvert<std::uint32_t>::Conv(count) / 16;
  }
  for (auto i = 0; i < count; i++) {
    if (*(raw.begin() + 8 + (16 * i)) == 0x01) {
      const auto t = srp::data::Convert<ara::com::someip::ServiceEntry>::Conv(
          std::vector<uint8_t>{raw.begin() + 8 + (16 * i), raw.end()});
      if (t.HasValue()) {
        this->AddNewOfferService(t.Value(), pid);
      }
    } else if (*(raw.begin() + 8 + (16 * i)) == 0x00) {
      const auto t = srp::data::Convert<ara::com::someip::ServiceEntry>::Conv(
          std::vector<uint8_t>{raw.begin() + 8 + (16 * i), raw.end()});
      if (t.HasValue()) {
        this->AddNewFindService(t.Value(), pid);
      } else {
        ara::log::LogError() << "Parssing error";
      }
    } else if (*(raw.begin() + 8 + (16 * i)) == 0x06) {
      // subscribe to event
    } else if (*(raw.begin() + 8 + (16 * i)) == 0x06) {
      // subscribe to event ACK
    } else {
      ara::log::LogError() << "unknow type";
    }
  }
}

void SDConnector::AddNewFindService(const ara::com::someip::ServiceEntry& entry,
                                    const uint32_t pid) {
  const auto& s_id = entry.service_id;
  const auto& i_id = entry.instance_id;
  const uint16_t key{s_id};
  const auto item =
      db::FindServiceItem{s_id, i_id, entry.major_version, entry.minor_version};
  sd_db_.AddNewConsumeService(std::move(item));
  ara::log::LogError() << "Offer 1";
  const auto i = sd_db_.FindInFindService(s_id, i_id);
  if (!i.has_value()) {
    std::runtime_error("Consume Service not exist in DB");
  }
  const auto& pid_iter = this->consume_service_list_.find(pid);
  if (pid_iter != consume_service_list_.end()) {
    ara::log::LogError() << "Offer 2";
    const auto& service_list = pid_iter->second;
    if (!pid_iter->second.contains(s_id)) {
      ara::log::LogError() << "Offer 3";
      pid_iter->second.insert({s_id, {i.value().get().ip_ != 0, i.value()}});
    }
  } else {
    ara::log::LogError() << "Offer 4";
    auto temp = decltype(consume_service_list_)::value_type{
        pid, decltype(consume_service_list_)::mapped_type{
                 std::pmr::new_delete_resource()}};
    temp.second.insert({s_id, {(i.value().get().ip_ != 0), i.value()}});
    consume_service_list_.insert(std::move(temp));
    ara::log::LogError() << "Offer 4: "
                         << static_cast<uint32_t>(consume_service_list_.size());
  }
  if (i.value().get().ip_ != 0) {
    this->SendIPCSdOffer(pid, i.value());
  }
}

void SDConnector::AddNewOfferService(
    const ara::com::someip::ServiceEntry& entry, const uint32_t pid) {
  const auto& s_id = entry.service_id;
  const auto& i_id = entry.instance_id;
  const uint16_t key{s_id};
  if (this->provider_service_list_.insert({key, pid}).second) {
    // logger_.LogInfo() << "[" << ip_ << ":" << port_ << "] New service: " <<
    // s_id
    //                   << ":" << i_id << " is provide by " << pid;
    sd_db_.AddNewProvideService(db::ServiceItem{s_id, i_id, entry.major_version,
                                                entry.minor_version, local_ip_,
                                                port_});
  } else {
    // logger_.LogError() << "[" << ip_ << ":" << port_ << "] Service: " << s_id
    //                    << ":" << i_id << " is already provide by "
    //                    << service_list_.at(key);
  }
}
void SDConnector::SdLoop(std::stop_token token) {
  while (!token.stop_requested()) {
    ara::log::LogError() << "[" << local_ip_ << ":" << port_ << "]: Loop start";
    sd_db_.WaitForNewService(token);
    if (token.stop_requested()) {
      break;
    }
    ara::log::LogError() << "[" << local_ip_ << ":" << port_
                         << "]: Loop triggered";
    for (auto& pid_item : this->consume_service_list_) {
      for (auto& service_item : pid_item.second) {
        if (!service_item.second.provided_) {
          if (service_item.second.sd_object_.get().ip_ != 0) {
            SendIPCSdOffer(pid_item.first, service_item.second.sd_object_);
            service_item.second.provided_ = true;
          }
        }
      }
    }
  }
}

void SDConnector::SendIPCSdOffer(
    uint32_t pid, std::reference_wrapper<const db::FindServiceItem> item) {
  const auto& item_ = item.get();
  ara::com::someip::SomeipSdFrameBuilder builder{};
  builder.AddOfferEntry(item_.service_id_, item_.instance_id_,
                        item_.major_version_, item_.minor_version_, item_.ip_,
                        item_.port_);
  std::vector<uint8_t> raw{0x01};
  const auto temp_v = builder.BuildFrame().GetRaw();
  raw.insert(raw.end(), temp_v.begin(), temp_v.end());
  this->ipc_soc_->TransmitToPid(pid, std::move(raw));
}

}  // namespace connectors
}  // namespace someip_demon
}  // namespace srp
