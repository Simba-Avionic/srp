/**
 * @file transfer.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_TRANSFER_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_TRANSFER_H_
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <vector>

#include "core/results/result.h"
namespace simba {
namespace com {
namespace someip {
namespace data {
class Transfer {
 private:
  std::vector<uint8_t> respons{};
  std::atomic_bool responsed{false};
  std::condition_variable cv{};
  const uint16_t transfer_id_;
  std::mutex cv_m;

 public:
  const uint16_t GetTransferID() const { return transfer_id_; }
  const bool IsRespond() const { return responsed; }
  explicit Transfer(const uint16_t transfer_id) : transfer_id_{transfer_id} {}
  void SetResponsed(std::vector<uint8_t> payload) {
    std::copy(payload.begin(), payload.end(), std::back_inserter(respons));
    responsed = true;
    cv.notify_one();
  }

  std::vector<uint8_t> GetPayload() const { return this->respons; }
  simba::core::Result<std::vector<uint8_t>> GetPayloadAsc() {
    std::unique_lock<std::mutex> lk(cv_m);
    if (!cv.wait_for(lk, std::chrono::seconds{2},
                     [this]() { return this->IsRespond(); })) {
      return simba::core::Result<std::vector<uint8_t>>{};
    }
    return simba::core::Result<std::vector<uint8_t>>{respons};
  }

  simba::core::Result<bool> GetACK() {
    std::unique_lock<std::mutex> lk(cv_m);
    if (!cv.wait_for(lk, std::chrono::seconds{2},
                     [this]() { return this->IsRespond(); })) {
      return simba::core::Result<bool>{};
    } else {
      return simba::core::Result<bool>{true};
    }
  }
};
}  // namespace data
}  // namespace someip
}  // namespace com
}  // namespace simba

#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_TRANSFER_H_
