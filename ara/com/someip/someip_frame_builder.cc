/**
 * @file someip_frame_builder.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/com/someip/someip_frame_builder.h"

#include <algorithm>
#include <utility>

namespace ara {
namespace com {
namespace someip {

SomeipFrameBuilder::SomeipFrameBuilder(/* args */) {
  header_.protocol_version = 1U;
  header_.interface_version = 1U;
  header_.length = 8U;
}

SomeipFrameBuilder& SomeipFrameBuilder::SetServiceId(const uint16_t& id) {
  this->header_.service_id = id;
  return *this;
}
SomeipFrameBuilder& SomeipFrameBuilder::SetMethodId(const uint16_t& id) {
  this->header_.method_id = id;
  return *this;
}
SomeipFrameBuilder& SomeipFrameBuilder::SetRequestId(const uint16_t& id) {
  this->header_.request_id = id;
  return *this;
}
SomeipFrameBuilder& SomeipFrameBuilder::SetSessionId(const uint16_t& id) {
  this->header_.session_id = id;
  return *this;
}
SomeipFrameBuilder& SomeipFrameBuilder::SetMessageType(
    const MessageType& type) {
  this->header_.message_type = static_cast<uint8_t>(type);
  return *this;
}
SomeipFrameBuilder& SomeipFrameBuilder::SetReturnCode(const MessageCode& code) {
  this->header_.return_code = static_cast<uint8_t>(code);
  return *this;
}
SomeipFrameBuilder& SomeipFrameBuilder::SetPayload(
    const std::vector<uint8_t>&& payload) {
  this->payload_ = std::move(payload);
  this->header_.length = 8U + this->payload_.size();
  return *this;
}

SomeipFrameBuilder& SomeipFrameBuilder::CopyFrameData(
    const HeaderStructure& header) {
  std::memcpy(&header_, &header, sizeof(header_));
  this->header_.length = 8U + this->payload_.size();
  return *this;
}

SomeipFrame SomeipFrameBuilder::BuildFrame() noexcept {
  return SomeipFrame(header_, payload_);
}

}  // namespace someip
}  // namespace com
}  // namespace ara
