/**
 * @file endpoint.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_ENDPOINT_H_
#define COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_ENDPOINT_H_
#include <cstdint>

namespace simba {
namespace com {
namespace someip {
namespace objects {
class Endpoint {
 protected:
  const uint16_t service_id_;
  const uint16_t endpoint_id_;

 public:
  Endpoint(const uint16_t service_id, const uint16_t endpoint_id)
      : service_id_{service_id}, endpoint_id_{endpoint_id} {}
  uint16_t GetServiceId() const { return service_id_; }
  uint16_t GetEndpointId() const { return endpoint_id_; }
};
}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_ENDPOINT_H_
