/**
 * Copyright (c) 2024
 *
 * @file e220_driver_factory.hpp
 * @brief Fabryka drivera `ebyte_e220` dla MavlinkDaemona: plik konfiguracji -> E220RadioDriver
 * na prawdziwym UART (termios) i pinach sysfs GPIO. Blad konfiguracji nie rzuca ani nie zwraca
 * nullptr: powstaje driver, ktorego Start() zwraca kPersistent z czytelnym opisem (log daemona,
 * ponowienie po `restart_delay`).
 */
#ifndef CORE_RADIO_E220_E220_DRIVER_FACTORY_HPP_
#define CORE_RADIO_E220_E220_DRIVER_FACTORY_HPP_

#include <memory>
#include <string>

#include "ara/com/mavlink/radio_driver.h"
#include "core/radio/e220/e220_radio_driver.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

/// Id drivera w configu daemona (`links.<nazwa>.driver`).
constexpr const char* kE220DriverId = "ebyte_e220";

struct E220FactoryOptions {
  /// TYLKO build testowy pluginu: `pins.ctl_dir` (piny w plikach atrapy modemu) jest dozwolone.
  bool allow_ctl_dir{false};
  /// Katalog sysfs GPIO (testy podaja katalog tymczasowy).
  std::string sysfs_gpio_root{"/sys/class/gpio"};
  E220Timing timing;
};

/// Driver wczytuje plik configu przy kazdym Start() (poprawka pliku dziala po ponownym starcie lacza).
std::shared_ptr<ara::com::mavlink::IRadioDriver> CreateE220Driver(const std::string& config_path,
                                                                 const E220FactoryOptions& options = {});

/// Rejestruje `ebyte_e220` w rejestrze daemona (wolane z pluginu).
void RegisterE220Driver(ara::com::mavlink::IRadioDriverRegistry& registry,  // NOLINT(runtime/references)
                        E220FactoryOptions options = {});

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_E220_DRIVER_FACTORY_HPP_
