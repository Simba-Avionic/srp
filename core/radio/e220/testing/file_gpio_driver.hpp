/**
 * Copyright (c) 2024
 *
 * @file file_gpio_driver.hpp
 * @brief TEST ONLY: gpio::IGpioDriver on files in the ctl directory of the e220_sim.py simulator
 * (m0/m1 are written by the driver, aux by the simulator; "0"/"1"). The pins are numbered like
 * kCtlPins (1 = M0, 2 = M1, 3 = AUX). Lets the real E220Modem run with a real UART on the simulator
 * pty without GPIO.
 */
#ifndef CORE_RADIO_E220_TESTING_FILE_GPIO_DRIVER_HPP_
#define CORE_RADIO_E220_TESTING_FILE_GPIO_DRIVER_HPP_

#include <cstdio>
#include <fstream>
#include <string>
#include <utility>

#include "core/gpio/Igpio_driver.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace testing {

class FileGpioDriver final : public gpio::IGpioDriver {
 public:
  explicit FileGpioDriver(std::string ctl_dir) : dir_{std::move(ctl_dir)} {}

  core::ErrorCode initializePin(const uint16_t&, const gpio::direction_t&, bool = true) override {
    return core::ErrorCode::kOk;
  }
  core::ErrorCode setDirection(const uint16_t&, const gpio::direction_t&, bool = true) override {
    return core::ErrorCode::kOk;
  }
  core::ErrorCode unregisterPin(const uint16_t&, bool = true) override { return core::ErrorCode::kOk; }
  gpio::direction_t getDirection(const uint16_t& pin, bool = true) override {
    return pin == 3 ? gpio::direction_t::IN : gpio::direction_t::OUT;
  }

  core::ErrorCode setValue(const uint16_t& pin, const uint8_t& value, bool = true) override {
    const char* name = pin == 1 ? "m0" : (pin == 2 ? "m1" : nullptr);
    if (name == nullptr) {
      return core::ErrorCode::kError;
    }
    const std::string tmp = dir_ + "/." + name + ".drv";
    {
      std::ofstream out{tmp, std::ios::trunc};
      if (!out) {
        return core::ErrorCode::kError;
      }
      out << (value != 0 ? "1" : "0");
    }
    return std::rename(tmp.c_str(), (dir_ + "/" + name).c_str()) == 0 ? core::ErrorCode::kOk : core::ErrorCode::kError;
  }

  uint8_t getValue(const uint16_t& pin, bool = true) override {
    if (pin != 3) {
      return static_cast<uint8_t>(core::ErrorCode::kError);
    }
    std::ifstream in{dir_ + "/aux"};
    std::string v;
    if (!(in >> v)) {
      return static_cast<uint8_t>(core::ErrorCode::kError);
    }
    return v == "1" ? 1 : 0;
  }

 private:
  std::string dir_;
};

}  // namespace testing
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_TESTING_FILE_GPIO_DRIVER_HPP_
