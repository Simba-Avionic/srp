/**
 * @file Isomeip_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_ISOMEIP_CONTROLLER_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_ISOMEIP_CONTROLLER_H_
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "core/common/error_code.h"
#include "core/results/result.h"
#include "communication-core/someip/message_code.h"
namespace simba {
namespace com {
namespace someip {
using SomeIPMethod = std::function<simba::core::Result<
    std::pair<std::vector<uint8_t>, com::core::data::MessageCode>>(
    const std::vector<uint8_t> payload)>;
class ISomeIpController {
 public:
  virtual simba::core::Result<std::vector<uint8_t>> Request(
      const uint16_t service_id, const uint16_t method_id,
      const std::vector<uint8_t> payload) = 0;
  virtual bool RequestNoResponse(const uint16_t service_id,
                                 const uint16_t method_id,
                                 const std::vector<uint8_t> payload) = 0;
  virtual simba::core::ErrorCode AddMethod(const uint16_t method_id,
                                           SomeIPMethod callback) = 0;
  virtual simba::core::ErrorCode AddEventValue(
      const uint16_t event_id, const std::vector<uint8_t> payload) = 0;
  virtual simba::core::ErrorCode Init() = 0;
  virtual simba::core::ErrorCode LoadServiceList(const std::string& path) = 0;
};
}  // namespace someip
}  // namespace com
}  // namespace simba

#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_ISOMEIP_CONTROLLER_H_
