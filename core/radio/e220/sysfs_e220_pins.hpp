/**
 * Copyright (c) 2024
 *
 * @file sysfs_e220_pins.hpp
 * @brief IE220Pins na sysfs GPIO (`/sys/class/gpio`) BEZ ara::log i bez srp `IGpioDriver`
 * (plugin `.so` nie moze uzywac loggera hosta). M0/M1 = wyjscia, AUX = wejscie.
 */
#ifndef CORE_RADIO_E220_SYSFS_E220_PINS_HPP_
#define CORE_RADIO_E220_SYSFS_E220_PINS_HPP_

#include <cstdint>
#include <string>
#include <utility>

#include "core/radio/e220/i_e220_pins.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

class SysfsE220Pins final : public IE220Pins {
 public:
  /// `root` = katalog sysfs GPIO (domyslnie /sys/class/gpio; testy podaja katalog tymczasowy).
  SysfsE220Pins(std::uint16_t m0, std::uint16_t m1, std::uint16_t aux, std::string root = "/sys/class/gpio")
      : m0_{m0}, m1_{m1}, aux_{aux}, root_{std::move(root)} {}

  /// Eksportuje piny i ustawia kierunki; false + opis w `error` (pierwszy blad).
  bool Prepare(std::string* error) override;
  bool SetMode(bool m0, bool m1) override;
  bool ReadAux(bool* high) override;

 private:
  bool Export(std::uint16_t pin, const char* direction, std::string* error);
  bool WriteFile(const std::string& path, const std::string& value);
  std::string PinPath(std::uint16_t pin, const char* leaf) const;

  std::uint16_t m0_, m1_, aux_;
  std::string root_;
};

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_SYSFS_E220_PINS_HPP_
