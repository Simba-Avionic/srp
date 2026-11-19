/**
 * Copyright (c) 2024
 *
 * @file sysfs_e220_pins.hpp
 * @brief IE220Pins on sysfs GPIO (`/sys/class/gpio`) WITHOUT ara::log and without the srp
 * `IGpioDriver` (a plugin `.so` cannot use the host logger). M0/M1 = outputs, AUX = input.
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
  /// `root` = the sysfs GPIO directory (default /sys/class/gpio; tests pass a temporary directory).
  SysfsE220Pins(std::uint16_t m0, std::uint16_t m1, std::uint16_t aux, std::string root = "/sys/class/gpio")
      : m0_{m0}, m1_{m1}, aux_{aux}, root_{std::move(root)} {}

  /// Exports the pins and sets the directions; false + a description in `error` (the first failure).
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
