/**
 * Copyright (c) 2024
 *
 * @file radio_driver.cpp
 * @brief RadioDriver: IRadioDriver mechanics on top of an IRadioModem (see radio_driver.hpp).
 */
#include "core/radio/common/radio_driver.hpp"

#include <utility>

#include "ara/com/mavlink/transport_error.h"

namespace srp {
namespace core {
namespace radio {

using ara::com::mavlink::MakeTransportError;
using ara::com::mavlink::TransportErrc;

RadioDriver::RadioDriver(std::string name, std::shared_ptr<IRadioModem> modem, RadioDriverOptions options)
    : name_{std::move(name)},
      modem_{std::move(modem)},
      caps_{modem_->Caps()},
      options_{options},
      logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger("radio", "", ara::log::LogLevel::kInfo)} {}

RadioDriver::~RadioDriver() { Stop(); }

// ------------------------------------------------------------------ Start / Stop

ara::core::Result<void> RadioDriver::Start(ara::com::mavlink::IRadioListener& listener,
                                           std::stop_token token) noexcept {
  std::lock_guard<std::mutex> life{lifecycle_mutex_};
  if (rx_thread_.joinable() || tx_thread_.joinable()) {
    return MakeTransportError(TransportErrc::kInvalidArgument, name_ + ": Start() without Stop()");
  }
  if (token.stop_requested()) {
    return MakeTransportError(TransportErrc::kClosed, name_ + ": Start interrupted");
  }
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_ = std::stop_source{};
  }
  // The token passed to Start() (daemon shutdown) also interrupts the modem configuration.
  std::stop_callback on_caller_stop{token, [this]() {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }};
  std::stop_token st;
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    st = stop_.get_token();
  }
  const auto configured = modem_->Configure(st);
  if (!configured.HasValue() || st.stop_requested()) {
    modem_->Close();
    if (!configured.HasValue()) {
      logger_.LogWarn() << name_ << ": modem configuration failed: " << configured.Error().SupportData();
      return configured.Error();
    }
    return MakeTransportError(TransportErrc::kClosed, name_ + ": Start interrupted");
  }
  {
    std::lock_guard<std::mutex> lock{mutex_};
    slot_.reset();
    tx_ready_owed_ = false;
    fatal_.store(false);
    running_ = true;
  }
  listener_ = &listener;
  try {
    if (caps_.full_duplex) {
      rx_thread_ = std::thread{[this, t = st] { RxLoop(t); }};
      tx_thread_ = std::thread{[this, t = st] { TxLoop(t); }};
    } else {
      rx_thread_ = std::thread{[this, t = st] { HalfDuplexLoop(t); }};
    }
  } catch (...) {
    StopThreadsLocked();
    return MakeTransportError(TransportErrc::kRetryable, name_ + ": could not start the threads");
  }
  return {};
}

void RadioDriver::StopThreadsLocked() {
  {
    std::lock_guard<std::mutex> lock{mutex_};
    running_ = false;
  }
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }
  cv_.notify_all();
  if (rx_thread_.joinable()) {
    rx_thread_.join();
  }
  if (tx_thread_.joinable()) {
    tx_thread_.join();
  }
  modem_->Close();
}

void RadioDriver::Stop() noexcept {
  {
    // Interrupts a running Start() (the modem configuration may take seconds) before the lifecycle
    // mutex held by Start() is taken.
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }
  std::lock_guard<std::mutex> life{lifecycle_mutex_};
  StopThreadsLocked();
}

// ------------------------------------------------------------------ Send (non-blocking)

ara::core::Result<void> RadioDriver::Send(std::span<const std::uint8_t> frame) noexcept {
  std::lock_guard<std::mutex> lock{mutex_};
  if (!running_) {
    return MakeTransportError(TransportErrc::kClosed, name_ + ": stopped");
  }
  if (fatal_.load()) {
    return MakeTransportError(TransportErrc::kPersistent, name_ + ": radio unusable (fatal error)");
  }
  if (frame.empty()) {
    return {};
  }
  if (caps_.mtu != 0 && frame.size() > caps_.mtu) {
    return MakeTransportError(TransportErrc::kInvalidArgument,
                              name_ + ": frame of " + std::to_string(frame.size()) + " B > MTU " +
                                  std::to_string(caps_.mtu) + " B");
  }
  if (slot_.has_value()) {
    tx_ready_owed_ = true;
    return MakeTransportError(TransportErrc::kBusy, name_ + ": TX slot occupied");
  }
  slot_.emplace(frame.begin(), frame.end());
  cv_.notify_all();
  return {};
}

// ------------------------------------------------------------------ threads

void RadioDriver::Fatal(ara::core::ErrorCode e, const std::stop_token& token) {
  if (token.stop_requested() || fatal_.exchange(true)) {
    return;  // stopping (no callbacks) or already reported: one OnError(fatal) per failure
  }
  logger_.LogError() << name_ << ": radio failed: " << e.SupportData();
  {
    std::lock_guard<std::mutex> lock{mutex_};
    running_ = false;  // Send() returns an error until the daemon calls Stop()/Start()
  }
  {
    std::lock_guard<std::mutex> src_lock{stop_mutex_};
    stop_.request_stop();
  }
  cv_.notify_all();
  listener_->OnError(std::move(e), true);
}

bool RadioDriver::TransmitSlot(const std::stop_token& token) {
  std::vector<std::uint8_t> frame;
  {
    std::lock_guard<std::mutex> lock{mutex_};
    if (!slot_.has_value()) {
      return true;
    }
    frame = *slot_;  // the slot stays occupied until the modem is done with the frame
  }
  const auto r = modem_->Transmit(frame, token);
  if (token.stop_requested()) {
    return false;
  }
  if (!r.HasValue()) {
    Fatal(r.Error(), token);
    return false;
  }
  bool owed = false;
  {
    std::lock_guard<std::mutex> lock{mutex_};
    slot_.reset();
    owed = tx_ready_owed_;
    tx_ready_owed_ = false;
  }
  if (owed) {
    listener_->OnTxReady();  // outside the mutex: the listener may call Send() right away
  }
  return true;
}

bool RadioDriver::ReceiveOnce(const std::stop_token& token) {
  const auto r = modem_->Receive(options_.rx_slice, token);
  if (token.stop_requested()) {
    return false;
  }
  if (!r.HasValue()) {
    Fatal(r.Error(), token);
    return false;
  }
  const auto& data = r.Value();
  if (!data.empty()) {
    listener_->OnReceive(std::span<const std::uint8_t>(data.data(), data.size()));
  }
  if (const auto stats = modem_->TakeStats()) {
    if (token.stop_requested()) {
      return false;
    }
    listener_->OnLinkStats(*stats);
  }
  return true;
}

void RadioDriver::HalfDuplexLoop(std::stop_token token) {
  while (!token.stop_requested()) {
    if (!TransmitSlot(token) || !ReceiveOnce(token)) {
      return;
    }
  }
}

void RadioDriver::RxLoop(std::stop_token token) {
  while (!token.stop_requested()) {
    if (!ReceiveOnce(token)) {
      return;
    }
  }
}

void RadioDriver::TxLoop(std::stop_token token) {
  std::unique_lock<std::mutex> lock{mutex_};
  while (!token.stop_requested()) {
    if (!slot_.has_value()) {
      cv_.wait_for(lock, std::chrono::milliseconds(100));
      continue;
    }
    lock.unlock();
    if (!TransmitSlot(token)) {
      return;
    }
    lock.lock();
  }
}

}  // namespace radio
}  // namespace core
}  // namespace srp
