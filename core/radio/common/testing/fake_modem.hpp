/**
 * Copyright (c) 2024
 *
 * @file fake_modem.hpp
 * @brief TEST ONLY: an IRadioModem with an "air" on both sides, for the RadioDriver tests and for the
 * tests of drivers built on it. The test plays the other end of the air: PeerWrite() = bytes arriving
 * at the radio, PeerRead() = bytes the radio transmitted (this also frees room in the transmit buffer).
 *
 * Transmit() blocks while the transmit buffer (`tx_capacity` bytes) has no room for the whole frame,
 * like a real modem gated by AUX/CTS. Fault injection: FailConfigure/FailTransmit/FailReceive and
 * HoldTransmit (Transmit() blocks until released or stopped). The fake also checks the threading
 * rules of IRadioModem: a half-duplex fake counts Transmit()/Receive() calls that overlap
 * (overlaps()), a full-duplex fake counts concurrent Transmit()/Transmit() or Receive()/Receive().
 */
#ifndef CORE_RADIO_COMMON_TESTING_FAKE_MODEM_HPP_
#define CORE_RADIO_COMMON_TESTING_FAKE_MODEM_HPP_

#include <algorithm>
#include <atomic>
#include <chrono>  // NOLINT
#include <condition_variable>  // NOLINT
#include <cstdint>
#include <deque>
#include <mutex>  // NOLINT
#include <optional>
#include <span>  // NOLINT
#include <stop_token>  // NOLINT
#include <string>
#include <utility>
#include <vector>

#include "ara/com/mavlink/transport_error.h"
#include "core/radio/common/i_radio_modem.hpp"

namespace srp {
namespace core {
namespace radio {
namespace testing {

struct FakeModemOptions {
  RadioCaps caps{255, false, false};
  /// Bytes the "air" takes from Transmit() before the peer reads (Transmit() blocks beyond it).
  std::size_t tx_capacity{400};
  /// Largest chunk returned by one Receive().
  std::size_t rx_chunk{64};
};

class FakeModem final : public IRadioModem {
 public:
  explicit FakeModem(FakeModemOptions options = {}) : options_{options} {}

  // ---- IRadioModem
  ara::core::Result<void> Configure(std::stop_token) noexcept override {
    ++configure_calls_;
    std::lock_guard<std::mutex> lock{mutex_};
    if (configure_error_) {
      return *configure_error_;
    }
    return {};
  }

  ara::core::Result<void> Transmit(std::span<const std::uint8_t> frame, std::stop_token stop) noexcept override {
    Overlap guard{*this, true};
    ++transmit_entered_;
    std::unique_lock<std::mutex> lock{mutex_};
    const bool ok = cv_.wait(lock, stop, [&] {
      return transmit_error_.has_value() ||
             (!hold_transmit_ && tx_air_.size() + frame.size() <= options_.tx_capacity);
    });
    if (!ok) {
      return ara::com::mavlink::MakeTransportError(ara::com::mavlink::TransportErrc::kClosed, "fake: stopped");
    }
    if (transmit_error_) {
      return *transmit_error_;
    }
    tx_air_.insert(tx_air_.end(), frame.begin(), frame.end());
    ++transmitted_frames_;
    cv_.notify_all();
    return {};
  }

  ara::core::Result<std::vector<std::uint8_t>> Receive(std::chrono::milliseconds timeout,
                                                       std::stop_token stop) noexcept override {
    Overlap guard{*this, false};
    std::unique_lock<std::mutex> lock{mutex_};
    cv_.wait_for(lock, stop, timeout, [&] { return !rx_air_.empty() || receive_error_.has_value(); });
    if (receive_error_) {
      return *receive_error_;
    }
    const std::size_t n = std::min(rx_air_.size(), options_.rx_chunk);
    std::vector<std::uint8_t> out(rx_air_.begin(), rx_air_.begin() + static_cast<std::ptrdiff_t>(n));
    rx_air_.erase(rx_air_.begin(), rx_air_.begin() + static_cast<std::ptrdiff_t>(n));
    return out;
  }

  void Close() noexcept override { ++close_calls_; }

  RadioCaps Caps() const noexcept override { return options_.caps; }

  std::optional<ara::com::mavlink::RadioLinkStats> TakeStats() noexcept override {
    std::lock_guard<std::mutex> lock{mutex_};
    if (stats_.empty()) {
      return std::nullopt;
    }
    auto s = stats_.front();
    stats_.pop_front();
    return s;
  }

  // ---- the other end of the air (test side)
  void PeerWrite(const std::vector<std::uint8_t>& bytes) {
    std::lock_guard<std::mutex> lock{mutex_};
    rx_air_.insert(rx_air_.end(), bytes.begin(), bytes.end());
    cv_.notify_all();
  }

  /// Up to `want` transmitted bytes (>= 1 if anything arrived within `timeout_ms`); empty = nothing.
  std::vector<std::uint8_t> PeerRead(std::size_t want, std::uint32_t timeout_ms) {
    std::unique_lock<std::mutex> lock{mutex_};
    cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms), [&] { return !tx_air_.empty(); });
    const std::size_t n = std::min(want, tx_air_.size());
    std::vector<std::uint8_t> out(tx_air_.begin(), tx_air_.begin() + static_cast<std::ptrdiff_t>(n));
    tx_air_.erase(tx_air_.begin(), tx_air_.begin() + static_cast<std::ptrdiff_t>(n));
    cv_.notify_all();  // room for a blocked Transmit()
    return out;
  }

  /// Fresh link measurements for TakeStats() (a queue: every value is returned once).
  void PushStats(const ara::com::mavlink::RadioLinkStats& s) {
    std::lock_guard<std::mutex> lock{mutex_};
    stats_.push_back(s);
  }

  // ---- fault injection
  /// nullopt = no error (ErrorCode is not assignable, hence emplace/reset).
  void FailConfigure(const std::optional<ara::core::ErrorCode>& e) { SetError(&configure_error_, e); }
  void FailTransmit(const std::optional<ara::core::ErrorCode>& e) { SetError(&transmit_error_, e); }
  void FailReceive(const std::optional<ara::core::ErrorCode>& e) { SetError(&receive_error_, e); }
  /// While held, Transmit() blocks (until released or until the stop token fires).
  void HoldTransmit(bool hold) {
    std::lock_guard<std::mutex> lock{mutex_};
    hold_transmit_ = hold;
    cv_.notify_all();
  }

  // ---- counters
  int configure_calls() const { return configure_calls_.load(); }
  int close_calls() const { return close_calls_.load(); }
  /// Transmit() calls that have started (also the ones still blocked).
  int transmit_entered() const { return transmit_entered_.load(); }
  /// Frames taken by the air.
  int transmitted_frames() const {
    std::lock_guard<std::mutex> lock{mutex_};
    return transmitted_frames_;
  }
  /// Calls that broke the threading rules of IRadioModem (see the file comment).
  int overlaps() const { return overlaps_.load(); }

 private:
  void SetError(std::optional<ara::core::ErrorCode>* field, const std::optional<ara::core::ErrorCode>& e) {
    std::lock_guard<std::mutex> lock{mutex_};
    field->reset();
    if (e) {
      field->emplace(*e);
    }
    cv_.notify_all();
  }

  class Overlap {
   public:
    Overlap(FakeModem& m, bool tx) : m_{m}, tx_{tx} {
      auto& same = tx ? m.in_tx_ : m.in_rx_;
      auto& other = tx ? m.in_rx_ : m.in_tx_;
      const int s = ++same;
      const int o = other.load();
      if (s > 1 || (!m.options_.caps.full_duplex && o > 0)) {
        ++m.overlaps_;
      }
    }
    ~Overlap() { --(tx_ ? m_.in_tx_ : m_.in_rx_); }
    Overlap(const Overlap&) = delete;
    Overlap& operator=(const Overlap&) = delete;

   private:
    FakeModem& m_;
    bool tx_;
  };

  const FakeModemOptions options_;
  mutable std::mutex mutex_;
  std::condition_variable_any cv_;
  std::deque<std::uint8_t> tx_air_;
  std::deque<std::uint8_t> rx_air_;
  std::deque<ara::com::mavlink::RadioLinkStats> stats_;
  std::optional<ara::core::ErrorCode> configure_error_;
  std::optional<ara::core::ErrorCode> transmit_error_;
  std::optional<ara::core::ErrorCode> receive_error_;
  bool hold_transmit_{false};
  int transmitted_frames_{0};
  std::atomic<int> configure_calls_{0}, close_calls_{0}, transmit_entered_{0}, overlaps_{0}, in_tx_{0}, in_rx_{0};
};

}  // namespace testing
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_COMMON_TESTING_FAKE_MODEM_HPP_
