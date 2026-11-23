/**
 * Copyright (c) 2024
 *
 * @file e220_modem.hpp
 * @brief Ebyte E220-400T30S as an IRadioModem (core/radio/common): all knowledge about the module -
 * registers, modes M0/M1, the AUX gate, portions of at most `packet_length` bytes. The threads, the TX
 * slot, kBusy/OnTxReady and the fatal-error handling are in RadioDriver (the `ebyte_e220` driver =
 * RadioDriver + E220Modem, see e220_driver_factory.hpp).
 *
 * The hardware goes through the srp interfaces: the UART through uart::IUartDriver (UartDriver, a
 * simulation in the sim build), M0/M1/AUX through gpio::IGpioDriver (a simulation in the sim build);
 * logs through ara::log.
 *
 * Configure(): configuration mode (M0=M1=1, UART 9600 8N1) -> read registers 00H-05H (C1) -> write (C0)
 * only the registers that differ from the target -> verifying read -> normal mode (M0=M1=0) -> UART at
 * the REG0 speed. 3 attempts, then a kRetryable error.
 *
 * Transmit(): one portion of at most `packet_length` bytes is written only while AUX is idle, and after
 * the write the modem waits for AUX busy -> idle (the module took the data and sent it) before the next
 * portion (manual 5.2.3). An AUX that does not follow, or a UART write error = the module is unusable
 * (kPersistent). Half duplex (the module cannot receive while it transmits): RadioDriver alternates
 * Receive() and Transmit() on one thread.
 *
 * Statistics (OPTIONAL, `stats_period_ms` > 0, off by default; UNVERIFIED on a real module): REG1 bit 5
 * is set (manual 6.3, p. 33: "RSSI Ambient Noise Enable") and every `stats_period_ms` the modem sends
 * `C0 C1 C2 C3 00 02` in the transparent mode; the module answers `C1 00 02 <noise> <last packet RSSI>`
 * (dBm = -(256 - value)) in the UART stream. The reply is cut out of the received bytes (a short window
 * after the command) and reported through TakeStats().
 */
#ifndef CORE_RADIO_E220_E220_MODEM_HPP_
#define CORE_RADIO_E220_E220_MODEM_HPP_

#include <chrono>  // NOLINT
#include <cstdint>
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <span>  // NOLINT
#include <stop_token>  // NOLINT
#include <string>
#include <vector>

#include "ara/com/mavlink/radio_driver.h"
#include "ara/log/log.h"
#include "core/gpio/Igpio_driver.hpp"
#include "core/radio/common/i_radio_modem.hpp"
#include "core/radio/e220/e220_registers.hpp"
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
  std::chrono::milliseconds read_slice{50};          ///< wait slice of the command replies
  int attempts{3};
};

/// GPIO numbers of the module pins (M0, M1: outputs; AUX: input).
struct E220Pins {
  std::uint16_t m0{0};
  std::uint16_t m1{0};
  std::uint16_t aux{0};
};

class E220Modem final : public IRadioModem {
 public:
  /// `stats_period` > 0 turns the RSSI/noise readout on (and REG1 bit 5 with it); 0 = off.
  E220Modem(std::string uart_port, E220Config config, E220Pins pins, std::shared_ptr<uart::IUartDriver> uart,
            std::shared_ptr<gpio::IGpioDriver> gpio, E220Timing timing = {},
            std::chrono::milliseconds stats_period = std::chrono::milliseconds{0});
  /// A modem with a configuration error: Configure() returns kPersistent with `config_error` (a readable
  /// daemon log); it touches no hardware.
  static std::shared_ptr<E220Modem> WithConfigError(std::string config_error);
  ~E220Modem() override;

  ara::core::Result<void> Configure(std::stop_token stop) noexcept override;
  ara::core::Result<void> Transmit(std::span<const std::uint8_t> frame, std::stop_token stop) noexcept override;
  ara::core::Result<std::vector<std::uint8_t>> Receive(std::chrono::milliseconds timeout,
                                                       std::stop_token stop) noexcept override;
  void Close() noexcept override;
  RadioCaps Caps() const noexcept override { return caps_; }
  std::optional<ara::com::mavlink::RadioLinkStats> TakeStats() noexcept override;

  /// The registers read most recently (diagnostics/tests).
  Registers last_registers() const;

 private:
  using Clock = std::chrono::steady_clock;

  /// One configuration attempt; nullopt = success, otherwise a description of the error.
  std::optional<std::string> ConfigureOnce(const std::stop_token& token);
  bool PreparePins(std::string* error);
  bool SetMode(bool m0, bool m1);
  /// Reads AUX (raw level); false = a pin error.
  bool ReadAux(bool* high);
  /// Waits until AUX is idle (`idle` = true) or busy (false); the idle level comes from the configuration.
  bool WaitAux(bool idle, std::chrono::milliseconds timeout, const std::stop_token& token);
  bool SwitchMode(bool config_mode, const std::stop_token& token, std::string* error);
  bool OpenUart(std::uint32_t baud, uart::Parity parity);
  void FlushRx();
  /// Sends a command and waits for `expected` bytes of the reply.
  std::optional<std::vector<std::uint8_t>> Exchange(const std::vector<std::uint8_t>& cmd, std::size_t expected,
                                                    const std::stop_token& token, std::string* error);
  bool ReadRegisters(Registers* regs, const std::stop_token& token, std::string* error);
  bool SleepFor(std::chrono::milliseconds d, const std::stop_token& token);

  /// Waits for AUX idle, writes one portion (<= packet_length) to the UART.
  ara::core::Result<void> WritePortion(std::span<const std::uint8_t> portion, const std::stop_token& token);
  /// After a write: waits for AUX busy and then idle again (the module took the data and sent it).
  ara::core::Result<void> AwaitModule(const std::stop_token& token);
  ara::core::Result<void> AuxFailed(const char* what, const std::stop_token& token);

  // Statistics (all from the Receive() thread, except stats_ which TakeStats() also reads from it).
  void MaybeRequestStats();
  /// Cuts the statistics reply out of `bytes` (the command is pending); returns what goes on to RadioDriver.
  std::vector<std::uint8_t> FilterStatsReply(std::vector<std::uint8_t> bytes);

  const std::string uart_port_;
  const E220Config config_;
  const E220Pins pins_;
  const E220Timing timing_;
  const std::chrono::milliseconds stats_period_;
  const RadioCaps caps_;
  std::string config_error_;
  std::shared_ptr<uart::IUartDriver> uart_;
  std::shared_ptr<gpio::IGpioDriver> gpio_;
  const ara::log::Logger& logger_;

  mutable std::mutex regs_mutex_;
  Registers last_registers_{};

  // statistics state
  Clock::time_point next_stats_request_{};
  Clock::time_point stats_sent_at_{};
  bool stats_pending_{false};
  bool stats_timeout_logged_{false};
  std::vector<std::uint8_t> stats_hold_;  ///< bytes held back while the reply may be split
  std::optional<ara::com::mavlink::RadioLinkStats> stats_;
};

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_E220_MODEM_HPP_
