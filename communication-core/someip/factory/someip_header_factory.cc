/**
 * @file someip_header_factory_imp.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "communication-core/someip/factory/someip_header_factory.h"

#include "communication-core/someip/message_code.h"
#include "communication-core/someip/message_type.h"

namespace simba {
namespace com {
namespace core {
namespace someip {
namespace factory {
std::shared_ptr<SomeIpHeader> SomeIpHeaderFactory::CreatEvent(
    const std::uint16_t service_id, const std::uint16_t event_id) {
  return std::make_shared<SomeIpHeader>(service_id, event_id, 0x00, 0x00, 0x00,
                                        0x1, data::kNotification, data::kEOk);
}

std::shared_ptr<SomeIpHeader> SomeIpHeaderFactory::CreateRequest(
    const std::uint16_t service_id, const std::uint16_t methode_id) {
  return std::make_shared<SomeIpHeader>(service_id, methode_id, 0x00, 0x00,
                                        0x00, 0x1, data::kRequest, data::kEOk);
}

std::shared_ptr<SomeIpHeader> SomeIpHeaderFactory::CreateRequestNoReturn(
    const std::uint16_t service_id, const std::uint16_t methode_id) {
  return std::make_shared<SomeIpHeader>(service_id, methode_id, 0x00, 0x00,
                                        0x00, 0x1, data::kRequestNoReturn, data::kEOk);
}

std::shared_ptr<SomeIpHeader> SomeIpHeaderFactory::CreateResponse(
    const std::uint16_t service_id, const std::uint16_t methode_id,
    const simba::com::core::data::MessageCode res_flag) {
  return std::make_shared<SomeIpHeader>(service_id, methode_id, 0x00, 0x00,
                                        0x00, 0x1, data::kResponse, res_flag);
}
}  // namespace factory
}  // namespace someip
}  // namespace core
}  // namespace com
}  // namespace simba
