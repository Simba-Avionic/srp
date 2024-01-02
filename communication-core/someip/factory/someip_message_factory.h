/**
 * @file someip_message_factory.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file defines a factory that creates all types of
 * messages on someIp
 * @version 1.0
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_FACTORY_SOMEIP_MESSAGE_FACTORY_H_
#define COMMUNICATION_CORE_SOMEIP_FACTORY_SOMEIP_MESSAGE_FACTORY_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "communication-core/someip/factory/Isomeip_message_factory.h"
#include "communication-core/someip/message_code.h"
#include "communication-core/someip/someip_header.h"
namespace simba {
namespace com {
namespace core {
namespace someip {
namespace factory {
class SomeIpMessageFactory : public ISomeIpMessageFactory {
 public:
  /**
   * @brief This function return ready bit stream to send
   *
   * @param header pointer at someipHeader
   * @param client_id client id
   * @param transfer_id transfer id
   * @param payload vector with payload
   * @return std::vector<uint8_t> vector with ready data to send
   */
  std::vector<uint8_t> GetBuffor(std::shared_ptr<SomeIpHeader> header,
                                 const uint16_t client_id,
                                 const uint16_t transfer_id,
                                 const std::vector<uint8_t> payload) override;
  /**
   * @brief Creat header object from raw data
   *
   * @param raw vector with bit stream
   * @return std::shared_ptr<SomeIpHeader> header
   */
  std::shared_ptr<SomeIpHeader> GetHeader(std::vector<uint8_t> raw) override;
  /**
   * @brief Get payload from raw data
   *
   * @param raw vector with bit stream
   * @return std::shared_ptr<SomeIpHeader> header
   */
  std::vector<uint8_t> GetPayload(std::vector<uint8_t> raw) override;
  ~SomeIpMessageFactory() = default;
};
}  // namespace factory
}  // namespace someip
}  // namespace core
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_FACTORY_SOMEIP_MESSAGE_FACTORY_H_
