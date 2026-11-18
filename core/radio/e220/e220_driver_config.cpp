/**
 * Copyright (c) 2024
 *
 * @file e220_driver_config.cpp
 */
#include "core/radio/e220/e220_driver_config.hpp"

#include <fstream>
#include <set>
#include <sstream>
#include <utility>

#include "nlohmann/json.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
DriverConfigResult Fail(std::string msg) {
  DriverConfigResult r;
  r.error = std::move(msg);
  return r;
}

bool PinNumber(const nlohmann::json& pins, const char* key, std::uint16_t* out, std::string* err) {
  if (!pins.contains(key)) {
    *err = std::string("pins: brak pola '") + key + "'";
    return false;
  }
  const auto& v = pins[key];
  if (!v.is_number_integer() || v.get<std::int64_t>() < 0 || v.get<std::int64_t>() > 65535) {
    *err = std::string("pins.") + key + " musi byc numerem GPIO (liczba calkowita 0..65535)";
    return false;
  }
  *out = static_cast<std::uint16_t>(v.get<std::int64_t>());
  return true;
}
}  // namespace

DriverConfigResult ParseDriverConfigJson(const std::string& text) {
  nlohmann::json j;
  try {
    j = nlohmann::json::parse(text);
  } catch (const std::exception& e) {
    return Fail(std::string("niepoprawny JSON: ") + e.what());
  }
  if (!j.is_object()) {
    return Fail("konfiguracja drivera musi byc obiektem JSON");
  }
  static const std::set<std::string> kKeys = {"uart", "pins", "e220"};
  for (const auto& kv : j.items()) {
    if (kKeys.count(kv.key()) == 0) {
      return Fail("nieznane pole '" + kv.key() + "' (dozwolone: uart, pins, e220)");
    }
  }
  DriverConfigResult r;
  if (!j.contains("uart") || !j["uart"].is_string() || j["uart"].get<std::string>().empty()) {
    return Fail("brak pola 'uart' (sciezka urzadzenia UART, np. \"/dev/ttyS2\")");
  }
  r.config.uart = j["uart"].get<std::string>();
  if (!j.contains("pins") || !j["pins"].is_object()) {
    return Fail("brak obiektu 'pins' ({\"m0\": N, \"m1\": N, \"aux\": N} — numery sysfs GPIO)");
  }
  const auto& pins = j["pins"];
  static const std::set<std::string> kPinKeys = {"m0", "m1", "aux", "ctl_dir"};
  for (const auto& kv : pins.items()) {
    if (kPinKeys.count(kv.key()) == 0) {
      return Fail("pins: nieznane pole '" + kv.key() + "' (dozwolone: m0, m1, aux)");
    }
  }
  std::string err;
  if (pins.contains("ctl_dir")) {
    if (!pins["ctl_dir"].is_string() || pins["ctl_dir"].get<std::string>().empty()) {
      return Fail("pins.ctl_dir musi byc niepustym stringiem");
    }
    r.config.pins.ctl_dir = pins["ctl_dir"].get<std::string>();
  } else {
    if (!PinNumber(pins, "m0", &r.config.pins.m0, &err) || !PinNumber(pins, "m1", &r.config.pins.m1, &err) ||
        !PinNumber(pins, "aux", &r.config.pins.aux, &err)) {
      return Fail(err);
    }
    if (r.config.pins.m0 == r.config.pins.m1 || r.config.pins.m0 == r.config.pins.aux ||
        r.config.pins.m1 == r.config.pins.aux) {
      return Fail("pins: m0, m1 i aux musza byc roznymi numerami GPIO");
    }
  }
  if (j.contains("e220")) {
    if (!j["e220"].is_object()) {
      return Fail("pole 'e220' musi byc obiektem (parametry radia)");
    }
    const auto radio = ParseConfigJson(j["e220"].dump());
    if (!radio.ok) {
      return Fail("e220: " + radio.error);
    }
    r.config.radio = radio.config;
  }
  const std::string invalid = Validate(r.config.radio);
  if (!invalid.empty()) {
    return Fail("e220: " + invalid);
  }
  r.ok = true;
  return r;
}

DriverConfigResult LoadDriverConfig(const std::string& path) {
  std::ifstream in{path};
  if (!in) {
    return Fail("nie mozna wczytac pliku konfiguracji '" + path + "'");
  }
  std::stringstream ss;
  ss << in.rdbuf();
  auto r = ParseDriverConfigJson(ss.str());
  if (!r.ok) {
    r.error = "plik '" + path + "': " + r.error;
  }
  return r;
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
