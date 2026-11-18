/**
 * Copyright (c) 2024
 *
 * @file e220_driver_config.hpp
 * @brief Plik konfiguracji drivera `ebyte_e220` (wskazywany przez `driver_config` w configu
 * MavlinkDaemona): UART, piny M0/M1/AUX i parametry radia. Format zna tylko driver.
 *
 * @code
 * {
 *   "uart": "/dev/ttyS2",
 *   "pins": { "m0": 66, "m1": 67, "aux": 69 },      // numery sysfs GPIO
 *   "e220": { "address": 0, "channel": 23, "air_rate": 2400, "power_dbm": 30,
 *             "uart_baudrate": 57600, "parity": "8N1", "packet_length": 200, "lbt": false }
 * }
 * @endcode
 * `e220` jest opcjonalne (domyslne wartosci jak w e220_registers.hpp). Nieznany klucz, brak
 * `uart`/`pins` albo wartosc spoza zakresu = blad z nazwa pola. `pins.ctl_dir` (piny w plikach
 * atrapy modemu) jest akceptowane przez parser, ale tylko w buildzie testowym pluginu.
 */
#ifndef CORE_RADIO_E220_E220_DRIVER_CONFIG_HPP_
#define CORE_RADIO_E220_E220_DRIVER_CONFIG_HPP_

#include <cstdint>
#include <optional>
#include <string>

#include "core/radio/e220/e220_registers.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

struct E220PinsConfig {
  std::uint16_t m0{0};
  std::uint16_t m1{0};
  std::uint16_t aux{0};
  std::string ctl_dir;  ///< TYLKO TESTY: katalog atrapy (m0/m1/aux jako pliki); niepusty = piny z plikow
};

struct E220DriverConfig {
  std::string uart;
  E220PinsConfig pins;
  E220Config radio;
};

struct DriverConfigResult {
  bool ok{false};
  E220DriverConfig config;
  std::string error;
};

DriverConfigResult ParseDriverConfigJson(const std::string& json_text);
/// Wczytuje i parsuje plik; blad zawiera sciezke pliku.
DriverConfigResult LoadDriverConfig(const std::string& path);

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_E220_DRIVER_CONFIG_HPP_
