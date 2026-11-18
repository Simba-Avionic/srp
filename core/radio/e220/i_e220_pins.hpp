/**
 * Copyright (c) 2024
 *
 * @file i_e220_pins.hpp
 * @brief Piny sterujace modemem E220: M0/M1 (wyjscia) i AUX (wejscie).
 */
#ifndef CORE_RADIO_E220_I_E220_PINS_HPP_
#define CORE_RADIO_E220_I_E220_PINS_HPP_

#include <string>

namespace srp {
namespace core {
namespace radio {
namespace e220 {

class IE220Pins {
 public:
  virtual ~IE220Pins() = default;
  /// Przygotowanie pinow (np. eksport sysfs GPIO); wolane na poczatku kazdej proby konfiguracji.
  /// false + opis w `error`. Domyslnie nic do zrobienia.
  virtual bool Prepare(std::string* error) {
    static_cast<void>(error);
    return true;
  }
  /// Ustawia M0/M1 (tryby: M1M0 = 00 transmisja, 11 konfiguracja). false = blad pinu.
  virtual bool SetMode(bool m0, bool m1) = 0;
  /// Odczyt AUX (true = wysoki = modul wolny). false = blad odczytu.
  virtual bool ReadAux(bool* high) = 0;
};

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_I_E220_PINS_HPP_
