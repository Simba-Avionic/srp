/**
 * Copyright (c) 2024
 *
 * @file file_e220_pins.hpp
 * @brief TESTONLY: IE220Pins na plikach w katalogu ctl atrapy e220_sim.py (m0/m1 zapisuje
 * sterownik, aux zapisuje atrapa; "0"/"1"). Pozwala uruchomic prawdziwy E220RadioDriver
 * z prawdziwym UartDriverem na pty atrapy bez GPIO.
 */
#ifndef CORE_RADIO_E220_TESTING_FILE_E220_PINS_HPP_
#define CORE_RADIO_E220_TESTING_FILE_E220_PINS_HPP_

#include <cstdio>
#include <fstream>
#include <string>
#include <utility>

#include "core/radio/e220/i_e220_pins.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace testing {

class FileE220Pins final : public IE220Pins {
 public:
  explicit FileE220Pins(std::string ctl_dir) : dir_{std::move(ctl_dir)} {}

  bool SetMode(bool m0, bool m1) override {
    return Write("m0", m0) && Write("m1", m1);
  }
  bool ReadAux(bool* high) override {
    std::ifstream in{dir_ + "/aux"};
    std::string v;
    if (!(in >> v)) {
      return false;
    }
    *high = v == "1";
    return true;
  }

 private:
  bool Write(const std::string& name, bool value) {
    const std::string tmp = dir_ + "/." + name + ".drv";
    {
      std::ofstream out{tmp, std::ios::trunc};
      if (!out) {
        return false;
      }
      out << (value ? "1" : "0");
    }
    return std::rename(tmp.c_str(), (dir_ + "/" + name).c_str()) == 0;
  }

  std::string dir_;
};

}  // namespace testing
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_TESTING_FILE_E220_PINS_HPP_
