/**
 * @file dtc_msg_factory.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "diag/dtc/factories/dtc_msg_factory.hpp"

#include <algorithm>
namespace simba {
namespace diag {
namespace dtc {

std::vector<uint8_t> DtcMsgFactory::GetBuffer(
  std::shared_ptr<diag::dtc::DtcHeader> header,
  std::vector<uint8_t> payload) {
  std::vector<std::uint8_t> res{header->GetBuffor()};
  std::copy(payload.begin(), payload.end(), std::back_inserter(res));
  return res;
}

std::shared_ptr<diag::dtc::DtcHeader> DtcMsgFactory::GetHeader(
    std::vector<uint8_t> raw_data) {
  auto header = std::make_shared<diag::dtc::DtcHeader>();
  header->SetBuffor(raw_data);
  return header;
}

std::vector<uint8_t> DtcMsgFactory::GetPayload(std::vector<uint8_t> raw_data) {
    std::vector<uint8_t> payload{};
    if (raw_data.size() > 4) {
    std::copy(raw_data.begin() + 0x4,
    raw_data.end(), std::back_inserter(payload));
    return payload;
    }
    return {};
}

}  // namespace dtc
}  // namespace diag
}  // namespace simba
