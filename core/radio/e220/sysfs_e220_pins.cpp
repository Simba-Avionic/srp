/**
 * Copyright (c) 2024
 *
 * @file sysfs_e220_pins.cpp
 */
#include "core/radio/e220/sysfs_e220_pins.hpp"

#include <sys/stat.h>

#include <fstream>
#include <tuple>
#include <utility>

namespace srp {
namespace core {
namespace radio {
namespace e220 {

std::string SysfsE220Pins::PinPath(std::uint16_t pin, const char* leaf) const {
  return root_ + "/gpio" + std::to_string(pin) + "/" + leaf;
}

bool SysfsE220Pins::WriteFile(const std::string& path, const std::string& value) {
  std::ofstream out{path};
  if (!out) {
    return false;
  }
  out << value;
  out.flush();
  return static_cast<bool>(out);
}

bool SysfsE220Pins::Export(std::uint16_t pin, const char* direction, std::string* error) {
  struct stat st;
  if (::stat((root_ + "/gpio" + std::to_string(pin)).c_str(), &st) != 0) {
    // pin nie wyeksportowany: export (blad EBUSY, gdy juz jest, ignorujemy i sprawdzamy ponownie)
    std::ignore = WriteFile(root_ + "/export", std::to_string(pin));
    if (::stat((root_ + "/gpio" + std::to_string(pin)).c_str(), &st) != 0) {
      *error = "nie mozna wyeksportowac GPIO " + std::to_string(pin) + " (" + root_ + "/export)";
      return false;
    }
  }
  if (!WriteFile(PinPath(pin, "direction"), direction)) {
    *error = "nie mozna ustawic kierunku '" + std::string(direction) + "' GPIO " + std::to_string(pin);
    return false;
  }
  return true;
}

bool SysfsE220Pins::Prepare(std::string* error) {
  return Export(m0_, "out", error) && Export(m1_, "out", error) && Export(aux_, "in", error);
}

bool SysfsE220Pins::SetMode(bool m0, bool m1) {
  return WriteFile(PinPath(m0_, "value"), m0 ? "1" : "0") && WriteFile(PinPath(m1_, "value"), m1 ? "1" : "0");
}

bool SysfsE220Pins::ReadAux(bool* high) {
  std::ifstream in{PinPath(aux_, "value")};
  std::string v;
  if (!(in >> v)) {
    return false;
  }
  if (v != "0" && v != "1") {
    return false;  // wartosc spoza 0/1 nie moze byc AUX wysokim ("modul wolny")
  }
  *high = v == "1";
  return true;
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
