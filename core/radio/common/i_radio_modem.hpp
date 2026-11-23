/**
 * Copyright (c) 2024
 *
 * @file i_radio_modem.hpp
 * @brief The interface every radio of the srp repo implements (IRadioModem). It is plain sequential,
 * BLOCKING code ("do it and wait"): no threads, no callbacks, no queues. All of that lives once in
 * RadioDriver (radio_driver.hpp), which turns an IRadioModem into the platform IRadioDriver.
 *
 * Threading: RadioDriver calls Configure() on the daemon link thread (from Start()), Close() after
 * its threads have been joined, and Transmit()/Receive() from the driver thread(s):
 *  - half duplex (Caps().full_duplex == false): ONE thread calls Receive() and Transmit() in turn,
 *    never at the same time;
 *  - full duplex: Transmit() and Receive() may run at the same time on two threads (but never two
 *    Transmit() or two Receive() at once).
 * Every blocking method must return promptly (well under a second) once `stop` is requested.
 */
#ifndef CORE_RADIO_COMMON_I_RADIO_MODEM_HPP_
#define CORE_RADIO_COMMON_I_RADIO_MODEM_HPP_

#include <chrono>  // NOLINT
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>  // NOLINT
#include <stop_token>  // NOLINT
#include <vector>

#include "ara/com/mavlink/radio_driver.h"
#include "ara/core/result.h"

namespace srp {
namespace core {
namespace radio {

/// Static properties of the radio. Constant for the life of the object and valid before Configure().
struct RadioCaps {
  /// Largest frame (bytes) that Transmit() accepts and that goes over the air atomically; 0 = no limit.
  /// RadioDriver reports it as IRadioDriver::MaxFrameSize().
  std::size_t mtu{0};
  /// true: the radio delivers whole packets (one Receive() = one packet); false: a byte stream.
  bool packet_based{false};
  /// true: Transmit() and Receive() may run concurrently (two driver threads); false: one thread, in turn.
  bool full_duplex{false};
};

class IRadioModem {
 public:
  virtual ~IRadioModem() = default;

  /// Brings the modem into the working state (idempotent; may be called again after Close()).
  /// Blocking; returns an error (e.g. kRetryable, kPersistent, kClosed) when the modem cannot be set up.
  virtual ara::core::Result<void> Configure(std::stop_token stop) noexcept = 0;

  /// Sends the whole `frame` and returns when the modem accepted/sent it (gating by AUX/CTS/TxDone
  /// happens inside). An error = the radio is unusable (RadioDriver reports it as a fatal error).
  virtual ara::core::Result<void> Transmit(std::span<const std::uint8_t> frame,
                                           std::stop_token stop) noexcept = 0;

  /// Waits at most `timeout` for received data. An empty vector = nothing arrived (timeout, stop);
  /// otherwise a chunk of the stream (or one packet). An error = the radio is unusable.
  virtual ara::core::Result<std::vector<std::uint8_t>> Receive(std::chrono::milliseconds timeout,
                                                               std::stop_token stop) noexcept = 0;

  /// Releases the hardware (ports, pins). Idempotent.
  virtual void Close() noexcept = 0;

  virtual RadioCaps Caps() const noexcept = 0;

  /// OPTIONAL link measurements (RSSI, noise floor, SNR): return a value when there is fresh data,
  /// otherwise nullopt. Called by RadioDriver from the thread that calls Receive(), after every
  /// Receive(); must not block. A modem without measurements keeps the default.
  virtual std::optional<ara::com::mavlink::RadioLinkStats> TakeStats() noexcept { return std::nullopt; }
};

}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_COMMON_I_RADIO_MODEM_HPP_
