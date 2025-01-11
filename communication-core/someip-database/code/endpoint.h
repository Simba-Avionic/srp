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
#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>
namespace srp {
namespace com {
namespace someip {
namespace objects {
class Endpoint {
 protected:
  const uint16_t service_id_;
  const uint16_t endpoint_id_;
  const std::vector<uint16_t> access_list_;

 public:
  Endpoint(const uint16_t service_id, const uint16_t endpoint_id)
      : service_id_{service_id}, endpoint_id_{endpoint_id}, access_list_{} {}
  Endpoint(const uint16_t service_id, const uint16_t endpoint_id,
           const std::vector<uint16_t> access_list)
      : service_id_{service_id},
        endpoint_id_{endpoint_id},
        access_list_{std::move(access_list)} {}
  uint16_t GetServiceId() const { return service_id_; }
  uint16_t GetEndpointId() const { return endpoint_id_; }
  bool CanPass(const std::uint16_t& service_id) const {
    if (access_list_.size() == 0) {
      return true;
    }
    const auto res =
        std::find(access_list_.begin(), access_list_.end(), service_id);
    return res != access_list_.end();
  }
};
}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace srp
#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_ENDPOINT_H_
