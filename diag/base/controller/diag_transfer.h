/**
 * @file diag_transfer.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-04
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DIAG_BASE_CONTROLLER_DIAG_TRANSFER_H_
#define DIAG_BASE_CONTROLLER_DIAG_TRANSFER_H_

#include <atomic>
#include <memory>
#include <vector>

#include "communication-core/someip-controller/transfer.h"

namespace simba {
namespace diag {
class DiagTransfer : public com::someip::data::Transfer {
 protected:
  bool error{false};

 public:
  explicit DiagTransfer(const uint16_t transfer_id)
      : com::someip::data::Transfer{transfer_id} {}

  std::optional<std::vector<uint8_t>> GetPayloadAsc() {
    std::unique_lock<std::mutex> lk(cv_m);
    if (!cv.wait_for(lk, std::chrono::seconds{2},
                     [this]() { return this->IsRespond() || this->error; })) {
      return {};
    }
    if (!this->error) {
      return response;
    } else {
      return {};
    }
  }

  void NegativeResponse() {
    this->error = true;
    cv.notify_one();
  }
};
}  // namespace diag
}  // namespace simba
#endif  // DIAG_BASE_CONTROLLER_DIAG_TRANSFER_H_
