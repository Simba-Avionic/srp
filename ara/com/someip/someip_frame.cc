/**
 * @file someip_frame.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/com/someip/someip_frame.h"

#include "ara/com/someip/message_type.h"

namespace ara {
namespace com {
namespace someip {
ara::core::Result<SomeipFrame> SomeipFrame::MakeFrame(
    const std::vector<uint8_t>& raw_frame) {
  const auto header =
      srp::data::Convert<ara::com::someip::HeaderStructure>::Conv(raw_frame);
  if (!header.HasValue()) {
    return header.Error();
  }
  return SomeipFrame(
      header.Value(),
      std::vector<uint8_t>{raw_frame.begin() + 0x10, raw_frame.end()});
}
SomeipFrame SomeipFrame::MakeFrame(
    HeaderStructure& header, const std::vector<uint8_t>& payload) {  // NOLINT
  header.length = 0x08 + payload.size();
  return SomeipFrame(header, payload);
}

SomeipFrame SomeipFrame::MakeFrame(HeaderStructure& header) {
  header.length = 0x08;
  return SomeipFrame(header);
}

SomeipFrame SomeipFrame::MakeResponseFrame(
    const SomeipFrame& header, const std::vector<uint8_t>& payload) {
  someip::HeaderStructure new_header = header.header_;
  new_header.message_type = static_cast<uint8_t>(MessageType::kResponse);
  new_header.length = 0x08 + payload.size();
  return SomeipFrame(new_header, payload);
}
SomeipFrame SomeipFrame::MakeResponseFrameWithError(
    const SomeipFrame& header,
    const ara::com::someip::MessageCode& error_code) {
  someip::HeaderStructure new_header = header.header_;
  new_header.message_type = static_cast<uint8_t>(MessageType::kError);
  new_header.return_code = static_cast<uint8_t>(error_code);
  new_header.length = 0x08;
  return SomeipFrame(new_header);
}
SomeipFrame::SomeipFrame(const HeaderStructure& header,
                         const std::vector<uint8_t>& payload)
    : header_{header}, payload_{payload} {}

SomeipFrame::SomeipFrame(const HeaderStructure& header)
    : header_{header}, payload_{} {}

std::vector<uint8_t> SomeipFrame::GetRaw() const {
  auto res_vec =
      srp::data::Convert2Vector<ara::com::someip::HeaderStructure>::Conv(
          header_);
  res_vec.insert(res_vec.end(), payload_.begin(), payload_.end());
  return res_vec;
}

}  // namespace someip
}  // namespace com
}  // namespace ara
