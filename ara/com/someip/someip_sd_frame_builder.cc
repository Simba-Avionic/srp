/**
 * @file someip_sd_frame_builder.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/com/someip/someip_sd_frame_builder.h"

#include <algorithm>
#include <utility>

namespace ara {
namespace com {
namespace someip {

SomeipSdFrameBuilder::SomeipSdFrameBuilder(/* args */) {
  this->header_.service_id = 0xffffU;
  this->header_.method_id = 0x8100U;
  this->header_.session_id = 1U;
  this->header_.protocol_version = 1U;
  this->header_.interface_version = 1U;
  this->header_.length = 8U;
  this->header_.message_type = MessageType::kError;
  this->header_.return_code = MessageCode::kEOk;
}

SomeipSdFrameBuilder& SomeipSdFrameBuilder::SetRequestId(const uint16_t& id) {
  this->header_.request_id = id;
  return *this;
}
SomeipSdFrameBuilder& SomeipSdFrameBuilder::SetSessionId(const uint16_t& id) {
  this->header_.session_id = id;
  return *this;
}
SomeipSdFrameBuilder& SomeipSdFrameBuilder::AddFindEntry(
    const uint16_t& service_id, const uint16_t& instace_id,
    const uint8_t major_version, const uint32_t& minor_version) {
  this->header_.message_type = MessageType::kNotification;
  ServiceEntry entry{
      0U,    0U,           0U, 0U, service_id, instace_id, major_version,
      3600U, minor_version};
  this->AddEntry(std::move(entry));
  return *this;
}
SomeipSdFrameBuilder& SomeipSdFrameBuilder::AddOfferEntry(
    const uint16_t& service_id, const uint16_t& instace_id,
    const uint8_t major_version, const uint32_t& minor_version,
    const uint32_t ip, const uint16_t port) {
  this->header_.message_type = MessageType::kNotification;
  ServiceEntry entry{
      1U, 0U,           0U, 0x10U, service_id, instace_id, major_version,
      5U, minor_version};
  EndpointOption endpoint{0x09U, 0x04U, 0x00U, ip, 0U, 0x11U, port};
  this->AddEntry(std::move(entry), std::move(endpoint));
  return *this;
}
void SomeipSdFrameBuilder::AddEntry(ServiceEntry&& entry_) {
  const auto& res =
      std::find_if(this->entry_list_.begin(), this->entry_list_.end(),
                   [entry_](const ServiceEntry& l_entry) {
                     return entry_.type == l_entry.type &&
                            entry_.service_id == l_entry.service_id &&
                            entry_.instance_id == l_entry.instance_id;
                   });

  if (res == this->entry_list_.end()) {
    this->entry_list_.push_back(std::move(entry_));
  }
}
void SomeipSdFrameBuilder::AddEntry(ServiceEntry&& entry_,
                                    EndpointOption&& endpoint) {
  const auto& res =
      std::find_if(this->entry_list_.begin(), this->entry_list_.end(),
                   [entry_](const ServiceEntry& l_entry) {
                     return entry_.type == l_entry.type &&
                            entry_.service_id == l_entry.service_id &&
                            entry_.instance_id == l_entry.instance_id;
                   });
  const auto& endpoint_iter =
      std::find_if(this->endpoint_list_.begin(), this->endpoint_list_.end(),
                   [endpoint](const auto& l_endpoint) {
                     return endpoint.ip == l_endpoint.ip &&
                            endpoint.port == l_endpoint.port &&
                            endpoint.protocol_type == l_endpoint.protocol_type;
                   });

  if (res == this->entry_list_.end()) {
    if (endpoint_iter == this->endpoint_list_.end()) {
      entry_.index_1 = this->endpoint_list_.size();
      this->endpoint_list_.push_back(std::move(endpoint));
    } else {
      entry_.index_1 = (endpoint_iter - endpoint_list_.begin());
    }
    this->entry_list_.push_back(std::move(entry_));
  }
}
uint8_t SomeipSdFrameBuilder::GetSize() const {
  return static_cast<uint8_t>(this->entry_list_.size());
}

SomeipFrame SomeipSdFrameBuilder::BuildFrame() noexcept {
  const uint8_t res1{0x00};
  const uint8_t res2{0x00};
  const uint8_t res3{0x00};
  const uint32_t entry_length{16U *
                              static_cast<uint32_t>(this->entry_list_.size())};

  std::vector<uint8_t> payload{flag, res1, res2, res3};

  if constexpr (std::endian::native == std::endian::big) {
    const auto res = srp::data::Convert2Vector<uint32_t>::Conv(entry_length);
    payload.insert(payload.end(), res.begin(), res.end());
  } else {
    const auto temp_v =
        srp::data::EndianConvert<std::uint32_t>::Conv(entry_length);
    const auto res = srp::data::Convert2Vector<uint32_t>::Conv(temp_v);
    payload.insert(payload.end(), res.begin(), res.end());
  }

  for (const auto& item : this->entry_list_) {
    const auto t =
        srp::data::Convert2Vector<ara::com::someip::ServiceEntry>::Conv(item);
    payload.insert(payload.end(), t.begin(), t.end());
  }
  if (this->endpoint_list_.size() > 0) {
    const uint32_t endpoint_length{
        12U * static_cast<uint32_t>(endpoint_list_.size())};

    if constexpr (std::endian::native == std::endian::big) {
      const auto res =
          srp::data::Convert2Vector<uint32_t>::Conv(endpoint_length);
      payload.insert(payload.end(), res.begin(), res.end());
    } else {
      const auto temp_v =
          srp::data::EndianConvert<std::uint32_t>::Conv(endpoint_length);
      const auto res = srp::data::Convert2Vector<uint32_t>::Conv(temp_v);
      payload.insert(payload.end(), res.begin(), res.end());
    }

    for (const auto& item : this->endpoint_list_) {
      const auto t =
          srp::data::Convert2Vector<ara::com::someip::EndpointOption>::Conv(
              item);
      payload.insert(payload.end(), t.begin(), t.end());
    }
  }
  header_.length = 8U + payload.size();
  return SomeipFrame{header_, payload};
}
}  // namespace someip
}  // namespace com
}  // namespace ara
