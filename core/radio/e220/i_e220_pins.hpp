/**
 * Copyright (c) 2024
 *
 * @file i_e220_pins.hpp
 * @brief Pins controlling the E220 modem: M0/M1 (outputs) and AUX (input).
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
  /// Prepares the pins (e.g. sysfs GPIO export); called at the start of every configuration attempt.
  /// false + a description in `error`. Does nothing by default.
  virtual bool Prepare(std::string* error) {
    static_cast<void>(error);
    return true;
  }
  /// Sets M0/M1 (modes: M1M0 = 00 transmission, 11 configuration). false = pin error.
  virtual bool SetMode(bool m0, bool m1) = 0;
  /// Reads AUX (the raw level; which level means "idle" is set by `aux_idle_level`). false = read error.
  virtual bool ReadAux(bool* high) = 0;
};

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_I_E220_PINS_HPP_
