/**
 * @file uds_request.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_UDS_REQUEST_H_
#define APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_UDS_REQUEST_H_
#include <algorithm>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

namespace simba {
namespace diag_ota {
namespace uds {
class UdsRequest {
 protected:
  const uint8_t service_id_;
  const std::vector<uint8_t> payload_;

 public:
  UdsRequest(const uint8_t service_id, const std::vector<uint8_t> payload)
      : service_id_{service_id}, payload_{payload} {}

  uint8_t GetServiceId() const { return service_id_; }
  ~UdsRequest() = default;
};

class UdsRequestFactory {
 private:
  /* data */
 public:
  static std::optional<UdsRequest> ParseRequest(unsigned char* data,
                                                int length) {
    if (length < 3) {
      return {};
    }
    std::vector<uint8_t> payload{};
    std::copy(data + 1, &data[length - 1],
              std::back_inserter(payload));  // NOLINT
    return UdsRequest{data[0], std::move(payload)};
  }
};
}  // namespace uds
}  // namespace diag_ota
}  // namespace simba

#endif  // APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_UDS_REQUEST_H_
