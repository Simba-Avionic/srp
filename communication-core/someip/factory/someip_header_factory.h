/**
 * @file someip_message_factory.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file defines  a factory that creates all types of
 * headers on someIp
 * @version 1.0
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_FACTORY_SOMEIP_HEADER_FACTORY_H_
#define COMMUNICATION_CORE_SOMEIP_FACTORY_SOMEIP_HEADER_FACTORY_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "communication-core/someip/factory/Isomeip_header_factory.h"
#include "communication-core/someip/message_code.h"
#include "communication-core/someip/someip_header.h"
namespace simba {
namespace com {
namespace core {
namespace someip {
namespace factory {
class SomeIpHeaderFactory : public ISomeIpHeaderFactory {
 public:
  /**
   * @brief This method will create a pre-prepared header na for messages of
   * type event
   *
   * @param service_id source service
   * @param event_id event id >0x8000
   * @return std::shared_ptr<SomeIpHeader> pre-prepared header
   */
  std::shared_ptr<SomeIpHeader> CreatEvent(
      const std::uint16_t service_id, const std::uint16_t event_id) override;
  /**
   * @brief This method will create a pre-prepared header na for messages of
   * type event
   *
   * @param service_id source service
   * @param event_id method id < 0x8000
   * @return std::shared_ptr<SomeIpHeader> pre-prepared header
   */
  std::shared_ptr<SomeIpHeader> CreateRequest(
      const std::uint16_t service_id, const std::uint16_t methode_id) override;
  /**
   * @brief This method will create a pre-prepared header na for response
   *
   * @param service_id source service
   * @param methode_id method id < 0x8000
   * @param res_flag result code
   * @return std::shared_ptr<SomeIpHeader>
   */
  std::shared_ptr<SomeIpHeader> CreateResponse(
      const std::uint16_t service_id, const std::uint16_t methode_id,
      const simba::com::core::data::MessageCode res_flag) override;
  std::shared_ptr<SomeIpHeader> CreateRequestNoReturn(
      const std::uint16_t service_id, const std::uint16_t methode_id) override;
  ~SomeIpHeaderFactory() = default;
};
}  // namespace factory
}  // namespace someip
}  // namespace core
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_FACTORY_SOMEIP_HEADER_FACTORY_H_
