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
#ifndef PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_UDS_REQUEST_H_
#define PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_UDS_REQUEST_H_
#include <algorithm>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

namespace srp {
namespace platform {
namespace diag_demon {
namespace uds {
class UdsRequest {
 protected:
  const uint8_t service_id_;
  const std::vector<uint8_t> payload_;

 public:
  UdsRequest(const uint8_t service_id, const std::vector<uint8_t> payload)
      : service_id_{service_id}, payload_{payload} {}

  uint8_t GetServiceId() const { return service_id_; }
  const std::vector<uint8_t>& GetPayload() const { return payload_; }
  ~UdsRequest() = default;
};

class UdsRequestFactory {
 private:
  /* data */
 public:
  static std::optional<UdsRequest> ParseRequest(unsigned char* data,
                                                int length) {
    if (length < 2) {
      return {};
    }
    std::vector<uint8_t> payload{};
    std::copy(data + 1, &data[length],
              std::back_inserter(payload));  // NOLINT
    return UdsRequest{data[0], std::move(payload)};
  }

  static std::vector<uint8_t> ParseRequestToLocalMSG(UdsRequest req) {
    std::vector<uint8_t> res{req.GetServiceId()};
    std::copy(req.GetPayload().cbegin(), req.GetPayload().cend(),
              std::back_inserter(res));
    return res;
  }
};
}  // namespace uds
}  // namespace diag_demon
}  // namespace platform
}  // namespace srp

#endif  // PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_UDS_REQUEST_H_
