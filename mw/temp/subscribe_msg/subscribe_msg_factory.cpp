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
#include "mw/temp/subscribe_msg/subscribe_msg_factory.h"


#include <algorithm>
namespace simba {
namespace mw {
namespace temp {

namespace {
  constexpr auto HDR_SIZE = 0x02;
}

std::vector<uint8_t> SubMsgFactory::GetBuffer(std::shared_ptr<simba::mw::temp::SubscribeHeader> header) {
  return header->GetBuffor();
}

std::shared_ptr<simba::mw::temp::SubscribeHeader> SubMsgFactory::GetHeader(
    std::vector<uint8_t> raw_data) {
  if (raw_data.size() < HDR_SIZE) {
    return nullptr;
  }
  auto header = std::make_shared<simba::mw::temp::SubscribeHeader>();
  header->SetBuffor(raw_data);
  return header;
}

}  // namespace temp
}  // namespace mw
}  // namespace simba
