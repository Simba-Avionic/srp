/**
 * Copyright (c) 2024
 *
 * @file e220_radio_driver.hpp
 * @brief Ebyte E220-400T30S modem driver as an ara::com::mavlink::IRadioDriver (plugin `ebyte_e220`
 * for MavlinkDaemon).
 *
 * Start(): configuration mode (M0=M1=1, UART 9600 8N1) -> read registers 00H-05H (C1) -> write (C0)
 * only the registers that differ from the target -> verifying read -> normal mode (M0=M1=0) ->
 * UART at the REG0 speed -> driver threads. 3 attempts, then a kRetryable error.
 *
 * Event model (platform: radio_driver.h):
 *  - the RX thread waits for UART data (poll) and calls OnReceive with chunks of the stream,
 *  - Send() is NON-BLOCKING: a frame (<= 400 B) is accepted whole when it fits into the TX queue,
 *    otherwise kBusy, and the TX thread calls OnTxReady() once the queue has room again,
 *  - the TX thread is gated by AUX (manual 5.2.3): one portion of at most `packet_length` bytes is
 *    written only while AUX is idle, and after the write it waits for AUX busy -> idle (the module
 *    took the data and sent it) before the next portion,
 *  - loss of the UART or of the module (read/write error, AUX not following) = OnError(kPersistent, fatal).
 * After Stop() there are no callbacks (threads joined). The platform knows nothing about the modem:
 * all knowledge about registers and pins is here.
 */
#ifndef CORE_RADIO_E220_E220_RADIO_DRIVER_HPP_
#define CORE_RADIO_E220_E220_RADIO_DRIVER_HPP_

#include <atomic>
#include <chrono>  // NOLINT
#include <condition_variable>  // NOLINT
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <span>  // NOLINT
#include <stop_token>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/com/mavlink/radio_driver.h"
#include "core/radio/e220/e220_registers.hpp"
#include "core/radio/e220/i_e220_pins.hpp"
#include "core/uart/Iuart_driver.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

/// Timings (defaults follow the manual; tests shorten them).
struct E220Timing {
  /// Max wait for AUX to change (idle before a write, busy and idle again after it, mode switches).
  std::chrono::milliseconds aux_timeout{2000};
  std::chrono::milliseconds aux_poll{1};
  /// Max wait for AUX to drop after a mode change (5.2.4: ~10 ms)
  std::chrono::milliseconds mode_switch_wait{30};
  std::chrono::milliseconds mode_settle{5};          ///< pause after AUX idle
  std::chrono::milliseconds command_timeout{500};    ///< reply to a HEX command
  std::chrono::milliseconds retry_delay{100};
  std::chrono::milliseconds read_slice{50};          ///< RX wait slice (Stop() wakes it within this time)
  int attempts{3};
};

class E220RadioDriver final : public ara::com::mavlink::IRadioDriver {
 public:
  /// Largest accepted frame and capacity of the TX queue (bytes accepted by Send() and not yet written).
  static constexpr std::size_t kTxQueueBytes = 400;

  E220RadioDriver(std::string name, std::string uart_port, E220Config config,
                  std::shared_ptr<uart::IUartDriver> uart, std::shared_ptr<IE220Pins> pins,
                  E220Timing timing = {});
  /// A driver with a configuration error: Start() returns kPersistent with `config_error` (a readable daemon log).
  static std::shared_ptr<E220RadioDriver> WithConfigError(std::string name, std::string config_error);
  ~E220RadioDriver() override;

  ara::core::Result<void> Start(ara::com::mavlink::IRadioListener& listener,  // NOLINT(runtime/references)
                                std::stop_token token) noexcept override;
  void Stop() noexcept override;
  ara::core::Result<void> Send(std::span<const std::uint8_t> frame) noexcept override;
  std::string Name() const noexcept override { return "e220:" + name_; }

  /// The registers read most recently (diagnostics/tests).
  Registers last_registers() const;

 private:
  using Clock = std::chrono::steady_clock;

  /// One configuration attempt; nullopt = success, otherwise a description of the error.
  std::optional<std::string> ConfigureOnce(const std::stop_token& token);
  /// Waits until AUX is idle (`idle` = true) or busy (false); the idle level comes from the configuration.
  bool WaitAux(bool idle, std::chrono::milliseconds timeout, const std::stop_token& token);
  bool SwitchMode(bool config_mode, const std::stop_token& token, std::string* error);
  bool OpenUart(std::uint32_t baud, uart::Parity parity);
  void FlushRx();
  /// Sends a command and waits for `expected` bytes of the reply.
  std::optional<std::vector<std::uint8_t>> Exchange(const std::vector<std::uint8_t>& cmd,
                                                    std::size_t expected, const std::stop_token& token,
                                                    std::string* error);
  bool ReadRegisters(Registers* regs, const std::stop_token& token, std::string* error);
  bool SleepFor(std::chrono::milliseconds d, const std::stop_token& token);

  void RxLoop(std::stop_token token);
  void TxLoop(std::stop_token token);
  /// Waits for AUX idle, writes one portion (<= packet_length) to the UART and removes it from the queue.
  /// false = fatal error (reported) or stop.
  bool WritePortion(const std::vector<std::uint8_t>& portion, const std::stop_token& token);
  /// After a write: waits for AUX busy and then idle again (the module took the data and sent it).
  bool AwaitModule(const std::stop_token& token);
  bool AuxFailed(const char* what, const std::stop_token& token);
  void Fatal(ara::core::ErrorCode e);
  void StopThreadsLocked();

  const std::string name_;
  const std::string uart_port_;
  const E220Config config_;
  const E220Timing timing_;
  std::string config_error_;
  std::shared_ptr<uart::IUartDriver> uart_;
  std::shared_ptr<IE220Pins> pins_;

  std::mutex lifecycle_mutex_;  // Start/Stop
  std::mutex stop_mutex_;  // guards stop_ (Start replaces it, Stop/Fatal request a stop)
  std::stop_source stop_;
  std::thread rx_thread_;
  std::thread tx_thread_;
  ara::com::mavlink::IRadioListener* listener_{nullptr};

  std::mutex mutex_;  // TX state (queue, running_)
  std::condition_variable cv_;
  bool running_{false};
  std::atomic<bool> fatal_{false};
  std::deque<std::vector<std::uint8_t>> queue_;
  std::size_t front_offset_{0};  // bytes of queue_.front() already written
  std::size_t queued_bytes_{0};  // accepted by Send(), not yet written to the UART
  bool tx_ready_owed_{false};
  std::size_t busy_need_{0};  // size of the frame rejected with kBusy: OnTxReady once there is room for it

  mutable std::mutex regs_mutex_;
  Registers last_registers_{};
};

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_E220_RADIO_DRIVER_HPP_
