/**
 * @file someip_message_factory.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file defines a factory that creates all types of
 * messages on someIp
 * @version 1.0
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "communication-core/someip/factory/someip_message_factory.h"

#include <algorithm>
#include <iterator>
#include <memory>
namespace simba {
namespace com {
namespace core {
namespace someip {
namespace factory {

std::vector<uint8_t> SomeIpMessageFactory::GetBuffor(
    std::shared_ptr<SomeIpHeader> header, const uint16_t client_id,
    const uint16_t transfer_id, const std::vector<uint8_t> payload) {
  header->SetClientID(client_id);
  header->SetSessionID(transfer_id);
  header->SetLength(payload.size());
  std::vector<std::uint8_t> res{header->GetBuffor()};
  std::copy(payload.begin(), payload.end(), std::back_inserter(res));
  return res;
}

std::shared_ptr<SomeIpHeader> SomeIpMessageFactory::GetHeader(
    std::vector<uint8_t> raw) {
  auto header = std::make_shared<SomeIpHeader>();
  header->SetBuffor(raw);
  return header;
}

std::vector<uint8_t> SomeIpMessageFactory::GetPayload(
    std::vector<uint8_t> raw) {
  std::vector<uint8_t> payload{};
  std::copy(raw.begin() + 0x10, raw.end(), std::back_inserter(payload));
  return payload;
}
}  // namespace factory
}  // namespace someip
}  // namespace core
}  // namespace com
}  // namespace simba
