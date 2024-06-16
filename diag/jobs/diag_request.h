/**
 * @file diag_request.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DIAG_JOBS_DIAG_REQUEST_H_
#define DIAG_JOBS_DIAG_REQUEST_H_
#include <cstdint>
#include <utility>
#include <vector>
namespace simba {
namespace diag {
class DiagRequest {
 private:
  const std::uint8_t s_id_;
  const std::vector<uint8_t> data_;

 public:
  explicit DiagRequest(const std::vector<uint8_t>& data)
      : s_id_{data[0]}, data_{data.begin() + 1, data.end()} {}
  uint8_t GetServiceID() const { return this->s_id_; }
  std::vector<uint8_t> GetPayload(const uint8_t& padding) const {
    if ((data_.begin() + padding) < data_.end()) {
      std::vector<uint8_t> res{data_.begin() + padding, data_.end()};
      return std::move(res);
    }
    return {};
  }

  std::vector<uint8_t> GetPayload(const uint8_t& start,
                                  const uint8_t& stop) const {
    if ((data_.begin() + start) < data_.end()) {
      std::vector<uint8_t> res{data_.begin() + start, data_.begin() + stop};
      return std::move(res);
    }
    return {};
  }
  virtual ~DiagRequest() = default;
};
}  // namespace diag
}  // namespace simba

#endif  // DIAG_JOBS_DIAG_REQUEST_H_
