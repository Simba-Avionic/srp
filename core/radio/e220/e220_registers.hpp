/**
 * Copyright (c) 2024
 *
 * @file e220_registers.hpp
 * @brief Ebyte E220-400T30S registers (00H-05H), the radio configuration and the driver
 * configuration file (UART, pins, radio parameters).
 *
 * Source: Ebyte "E220-xxxTxxx Series User Manual", sections 6.1-6.4 (see README.md for the
 * exact document reference). Pure logic, no I/O except LoadDriverConfig().
 *
 * Driver configuration file (the `driver_config` of a MavlinkDaemon link; only the driver knows
 * this format):
 * @code
 * {
 *   "uart": "/dev/ttyS2",
 *   "pins": { "m0": 66, "m1": 67, "aux": 69 },      // sysfs GPIO numbers
 *   "aux_idle_level": 1,                            // AUX level when the module is idle (1 or 0)
 *   "stats_period_ms": 0,                           // RSSI/noise readout period, 0 = off (default)
 *   "e220": { "address": 0, "channel": 23, "air_rate": 2400, "power_dbm": 30,
 *             "uart_baudrate": 57600, "parity": "8N1", "packet_length": 200, "lbt": false }
 * }
 * @endcode
 * `e220`, `aux_idle_level` and `stats_period_ms` are optional (defaults: factory values, 1, 0). An unknown key, a
 * missing `uart`/`pins` or a value out of range is an error naming the field. `pins.ctl_dir`
 * (pins in the files of the module simulator) is accepted by the parser but honoured only by the
 * test build of the plugin.
 */
#ifndef CORE_RADIO_E220_E220_REGISTERS_HPP_
#define CORE_RADIO_E220_E220_REGISTERS_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace srp {
namespace core {
namespace radio {
namespace e220 {

constexpr std::size_t kRegisterCount = 6;  // 00H..05H (CRYPT 06H/07H are not used)
constexpr std::size_t kRegAddrH = 0;
constexpr std::size_t kRegAddrL = 1;
constexpr std::size_t kReg0 = 2;  // UART baud rate, parity, air rate
constexpr std::size_t kReg1 = 3;  // packet length, ambient RSSI, bit 2 (see README), power
constexpr std::size_t kReg2 = 4;  // channel
constexpr std::size_t kReg3 = 5;  // RSSI byte, fixed transmission, LBT, WOR

using Registers = std::array<std::uint8_t, kRegisterCount>;

enum class UartParity : std::uint8_t { k8N1 = 0, k8O1 = 1, k8E1 = 2 };

/// Radio configuration (what the driver configuration file sets). The remaining bits are forced.
struct E220Config {
  std::uint16_t address{0};
  std::uint8_t channel{23};          ///< 0..83 (410.125 + CH MHz)
  std::uint32_t air_rate_bps{2400};  ///< 2400 4800 9600 19200 38400 62500
  std::uint8_t power_dbm{30};        ///< 30 27 24 21 (E220-400T30S)
  std::uint32_t uart_baudrate{9600};  ///< 1200..115200 (REG0)
  UartParity parity{UartParity::k8N1};
  std::uint16_t packet_length{200};  ///< 200 128 64 32 B
  bool lbt{false};                   ///< REG3 bit4
  std::optional<std::uint8_t> wor_cycle;  ///< 0..7; none = left as it is (not managed)
  /// Level of the AUX pin while the module is idle (not a register; set in the driver file).
  bool aux_idle_level{true};
  /// REG1 bit5 (RSSI ambient noise enable, manual 6.3, p. 33): the module answers the C0 C1 C2 C3 read.
  /// Not a field of the `e220` JSON object: the driver sets it from `stats_period_ms` > 0.
  bool ambient_rssi{false};

  bool operator==(const E220Config&) const = default;
};

/// Empty string = valid; otherwise a description of the error (ranges of the E220-400T30S).
std::string Validate(const E220Config& cfg);

/// Parses JSON (a configuration object or an object with an "e220" key); an unknown key is an error.
struct ParseResult {
  bool ok{false};
  E220Config config;
  std::string error;
};
ParseResult ParseConfigJson(const std::string& json_text);

/// Target registers: `current` with the forced fields (REG3 transparent, no RSSI byte, no ambient
/// RSSI; reserved bits and REG1 bit 2 stay as read).
Registers DesiredRegisters(const E220Config& cfg, const Registers& current);

/// Whether register `reg` matches the target in the managed fields (equivalent codes, e.g.
/// air rate 2.4k = 000/001/010 and parity 11 = 00, are treated as equal).
bool RegisterMatches(std::size_t reg, std::uint8_t actual, std::uint8_t desired,
                     const E220Config& cfg);

/// Indices of the registers that differ from the target (only these are written with C0).
std::vector<std::size_t> DifferingRegisters(const Registers& actual, const Registers& desired,
                                            const E220Config& cfg);

/// Codes of single fields (nullopt = value out of range).
std::optional<std::uint8_t> BaudCode(std::uint32_t baud);
std::optional<std::uint8_t> AirRateCode(std::uint32_t bps);
std::optional<std::uint8_t> PowerCode(std::uint8_t dbm);
std::optional<std::uint8_t> PacketCode(std::uint16_t bytes);

/// Register description for logs, e.g. "addr=0x0000 uart=9600 8N1 air=2400 ch=23 ...".
std::string DescribeRegisters(const Registers& regs);

struct E220PinsConfig {
  std::uint16_t m0{0};
  std::uint16_t m1{0};
  std::uint16_t aux{0};
  std::string ctl_dir;  ///< TEST ONLY: simulator directory (m0/m1/aux as files); non-empty = pins from files
};

struct E220DriverConfig {
  std::string uart;
  E220PinsConfig pins;
  E220Config radio;  ///< includes `aux_idle_level`
  /// Period of the RSSI/noise readout reported through OnLinkStats; 0 = off (default).
  std::uint32_t stats_period_ms{0};
};

struct DriverConfigResult {
  bool ok{false};
  E220DriverConfig config;
  std::string error;
};

DriverConfigResult ParseDriverConfigJson(const std::string& json_text);
/// Loads and parses a file; the error contains the file path.
DriverConfigResult LoadDriverConfig(const std::string& path);

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_E220_REGISTERS_HPP_
