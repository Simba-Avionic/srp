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
#include "mw/temp/subscribe_msg/subscribe_header.h"
#include "subscribe_msg_factory.h"


#include <algorithm>
namespace simba {
namespace mw {
namespace temp {

std::vector<uint8_t> SubMsgFactory::GetBuffer(
  std::shared_ptr<simba::mw::temp::SubscribeHeader> header,
  std::vector<uint8_t>&& payload) {
  std::vector<std::uint8_t> res{header->GetBuffor()};
  std::copy(payload.begin(), payload.end(), std::back_inserter(res));
  return res;
}

std::shared_ptr<simba::mw::temp::SubscribeHeader> SubMsgFactory::GetHeader(
    std::vector<uint8_t> raw_data) {

  auto header = std::make_shared<simba::mw::temp::SubscribeHeader>();
  header->SetBuffor(raw_data);
  return header;
}

std::vector<uint8_t> SubMsgFactory::GetPayload(std::vector<uint8_t> raw_data) {
    std::vector<uint8_t> payload{};
    if (raw_data.size() > 4) {
    std::copy(raw_data.begin() + 0x4,
    raw_data.end(), std::back_inserter(payload));
    return payload;
    }
    return {};
}

}  // namespace temp
}  // namespace mw
}  // namespace simba
