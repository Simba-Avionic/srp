/**
 * Copyright (c) 2024
 *
 * @file e220_registers.hpp
 * @brief Rejestry Ebyte E220-400T30S (00H-05H) i konfiguracja radia z JSON-a aplikacji.
 *
 * Zrodlo: Ebyte "E220-xxxTxxx Series User Manual" rev. 1.8 (2026-11-12), sekcje 6.1-6.4
 * (str. 30-35) — patrz README.md. Czysta logika, bez I/O.
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

constexpr std::size_t kRegisterCount = 6;  // 00H..05H (CRYPT 06H/07H nieuzywane)
constexpr std::size_t kRegAddrH = 0;
constexpr std::size_t kRegAddrL = 1;
constexpr std::size_t kReg0 = 2;  // baud UART, parzystosc, szybkosc radiowa
constexpr std::size_t kReg1 = 3;  // dlugosc pakietu, RSSI otoczenia, soft-switch, moc
constexpr std::size_t kReg2 = 4;  // kanal
constexpr std::size_t kReg3 = 5;  // bajt RSSI, transmisja fixed, LBT, WOR

using Registers = std::array<std::uint8_t, kRegisterCount>;

enum class UartParity : std::uint8_t { k8N1 = 0, k8O1 = 1, k8E1 = 2 };

/// Konfiguracja radia (to, co aplikacja ustawia w JSON-ie). Reszta bitow jest narzucona.
struct E220Config {
  std::uint16_t address{0};
  std::uint8_t channel{23};          ///< 0..83 (410.125 + CH MHz)
  std::uint32_t air_rate_bps{2400};  ///< 2400 4800 9600 19200 38400 62500
  std::uint8_t power_dbm{30};        ///< 30 27 24 21 (E220-400T30S)
  std::uint32_t uart_baudrate{9600};  ///< 1200..115200 (REG0)
  UartParity parity{UartParity::k8N1};
  std::uint16_t packet_length{200};  ///< 200 128 64 32 B
  bool lbt{false};                   ///< REG3 bit4
  std::optional<std::uint8_t> wor_cycle;  ///< 0..7; brak = bez zmian (nie zarzadzane)

  bool operator==(const E220Config&) const = default;
};

/// Pusty string = poprawna; inaczej opis bledu (zakresy E220-400T30S).
std::string Validate(const E220Config& cfg);

/// Parsuje JSON (obiekt konfiguracji albo obiekt z kluczem "e220"); nieznany klucz = blad.
struct ParseResult {
  bool ok{false};
  E220Config config;
  std::string error;
};
ParseResult ParseConfigJson(const std::string& json_text);

/// Docelowe rejestry: `current` z narzuconymi polami (REG3 transparent, bez bajtu RSSI,
/// bez RSSI otoczenia i soft-switch; bity zarezerwowane zostaja jak odczytane).
Registers DesiredRegisters(const E220Config& cfg, const Registers& current);

/// Czy rejestr `reg` jest zgodny z docelowym w polach zarzadzanych (kody rownowazne,
/// np. szybkosc 2.4k = 000/001/010, parzystosc 11 = 00, sa traktowane jako rowne).
bool RegisterMatches(std::size_t reg, std::uint8_t actual, std::uint8_t desired,
                     const E220Config& cfg);

/// Indeksy rejestrow roznych od docelowych (tylko one trafiaja do zapisu C0).
std::vector<std::size_t> DifferingRegisters(const Registers& actual, const Registers& desired,
                                            const E220Config& cfg);

/// Kody pojedynczych pol (nullopt = wartosc spoza zakresu).
std::optional<std::uint8_t> BaudCode(std::uint32_t baud);
std::optional<std::uint8_t> AirRateCode(std::uint32_t bps);
std::optional<std::uint8_t> PowerCode(std::uint8_t dbm);
std::optional<std::uint8_t> PacketCode(std::uint16_t bytes);

/// Opis rejestrow do logow, np. "ADDR=0x0000 baud=9600 8N1 air=2400 ch=23 ...".
std::string DescribeRegisters(const Registers& regs);

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_E220_REGISTERS_HPP_
