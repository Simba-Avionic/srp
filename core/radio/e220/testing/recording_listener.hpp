/**
 * Copyright (c) 2024
 *
 * @file recording_listener.hpp
 * @brief TESTONLY: IRadioListener zapamietujacy bajty i liczacy callbacki (po `Seal()` kazdy callback
 * to naruszenie kontraktu: zero callbackow po Stop()).
 */
#ifndef CORE_RADIO_E220_TESTING_RECORDING_LISTENER_HPP_
#define CORE_RADIO_E220_TESTING_RECORDING_LISTENER_HPP_

#include <atomic>
#include <cstdint>
#include <mutex>  // NOLINT
#include <span>  // NOLINT
#include <vector>

#include "ara/com/mavlink/radio_driver.h"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace testing {

class RecordingListener final : public ara::com::mavlink::IRadioListener {
 public:
  void OnReceive(std::span<const std::uint8_t> bytes) noexcept override {
    Note();
    std::lock_guard<std::mutex> lock{mutex_};
    received_.insert(received_.end(), bytes.begin(), bytes.end());
  }
  void OnTxReady() noexcept override {
    Note();
    ++tx_ready_;
  }
  void OnError(ara::core::ErrorCode, bool fatal) noexcept override {
    Note();
    ++errors_;
    if (fatal) {
      ++fatal_errors_;
    }
  }

  std::vector<std::uint8_t> received() const {
    std::lock_guard<std::mutex> lock{mutex_};
    return received_;
  }
  std::size_t received_size() const {
    std::lock_guard<std::mutex> lock{mutex_};
    return received_.size();
  }
  int tx_ready() const { return tx_ready_.load(); }
  int errors() const { return errors_.load(); }
  int fatal_errors() const { return fatal_errors_.load(); }
  void Seal() { sealed_ = true; }
  int violations() const { return violations_.load(); }

 private:
  void Note() {
    if (sealed_.load()) {
      ++violations_;
    }
  }
  mutable std::mutex mutex_;
  std::vector<std::uint8_t> received_;
  std::atomic<int> tx_ready_{0}, errors_{0}, fatal_errors_{0}, violations_{0};
  std::atomic<bool> sealed_{false};
};

}  // namespace testing
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_TESTING_RECORDING_LISTENER_HPP_
