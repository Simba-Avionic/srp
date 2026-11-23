/**
 * Copyright (c) 2024
 *
 * @file radio_driver.hpp
 * @brief RadioDriver: ONE implementation of the platform ara::com::mavlink::IRadioDriver mechanics for
 * every radio of the srp repo; the radio itself is an IRadioModem (blocking, sequential code).
 *
 * Contract (platform radio_driver.h), as implemented here:
 *  - Start(): IRadioModem::Configure() on the calling thread, then the driver thread(s);
 *    Start() after Stop() works.
 *  - Stop(): joins the threads; after it returns there are ZERO callbacks (also when it interrupts a
 *    running Transmit()/Receive()/Configure() through the stop token).
 *  - Send(): non-blocking; the TX queue holds ONE frame (it stays in the slot until Transmit() returns,
 *    so at most one outdated frame sits in the driver); a second frame gets kBusy and the driver calls
 *    OnTxReady() (once) when the slot is free again. A frame larger than Caps().mtu is
 *    kInvalidArgument; MaxFrameSize() == Caps().mtu.
 *  - A runtime error of the modem (Transmit/Receive) = OnError(fatal) EXACTLY ONCE; afterwards Send()
 *    fails with kPersistent until the daemon calls Stop() and Start().
 *  - OnLinkStats(): what IRadioModem::TakeStats() returns, from the RX thread.
 *  - Threads: half-duplex modem = one thread alternating Receive(rx_slice) and Transmit() (a frame
 *    waits at most one `rx_slice` for the thread); full-duplex modem = an RX and a TX thread.
 */
#ifndef CORE_RADIO_COMMON_RADIO_DRIVER_HPP_
#define CORE_RADIO_COMMON_RADIO_DRIVER_HPP_

#include <atomic>
#include <chrono>  // NOLINT
#include <condition_variable>  // NOLINT
#include <cstdint>
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <span>  // NOLINT
#include <stop_token>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/com/mavlink/radio_driver.h"
#include "ara/log/log.h"
#include "core/radio/common/i_radio_modem.hpp"

namespace srp {
namespace core {
namespace radio {

struct RadioDriverOptions {
  /// Length of one Receive() call. Stop() is noticed within this time (modems should also react to
  /// the stop token) and, for a half-duplex modem, a queued frame waits at most this long.
  std::chrono::milliseconds rx_slice{10};
};

class RadioDriver final : public ara::com::mavlink::IRadioDriver {
 public:
  RadioDriver(std::string name, std::shared_ptr<IRadioModem> modem, RadioDriverOptions options = {});
  ~RadioDriver() override;

  ara::core::Result<void> Start(ara::com::mavlink::IRadioListener& listener,  // NOLINT(runtime/references)
                                std::stop_token token) noexcept override;
  void Stop() noexcept override;
  ara::core::Result<void> Send(std::span<const std::uint8_t> frame) noexcept override;
  std::string Name() const noexcept override { return name_; }
  std::size_t MaxFrameSize() const noexcept override { return caps_.mtu; }

 private:
  void HalfDuplexLoop(std::stop_token token);
  void RxLoop(std::stop_token token);
  void TxLoop(std::stop_token token);
  /// Transmits the frame in the slot and frees it (then OnTxReady if owed). false = fatal error or stop.
  bool TransmitSlot(const std::stop_token& token);
  /// One Receive() + OnReceive/OnLinkStats. false = fatal error or stop.
  bool ReceiveOnce(const std::stop_token& token);
  void Fatal(ara::core::ErrorCode e, const std::stop_token& token);
  void StopThreadsLocked();

  const std::string name_;
  const std::shared_ptr<IRadioModem> modem_;
  const RadioCaps caps_;
  const RadioDriverOptions options_;
  const ara::log::Logger& logger_;

  std::mutex lifecycle_mutex_;  // Start/Stop
  std::mutex stop_mutex_;       // guards stop_ (Start replaces it, Stop/Fatal request a stop)
  std::stop_source stop_;
  std::thread rx_thread_;       // the only thread of a half-duplex modem
  std::thread tx_thread_;       // full duplex only
  ara::com::mavlink::IRadioListener* listener_{nullptr};

  std::mutex mutex_;  // TX slot and running_
  std::condition_variable cv_;
  bool running_{false};
  std::optional<std::vector<std::uint8_t>> slot_;  // the one queued frame
  bool tx_ready_owed_{false};
  std::atomic<bool> fatal_{false};
};

}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_COMMON_RADIO_DRIVER_HPP_
