/**
 * Copyright (c) 2024
 *
 * @file e220_registers.cpp
 */
#include "core/radio/e220/e220_registers.hpp"

#include <algorithm>
#include <cstdio>
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

// REG0 [7:5] UART baud rate (6.3, p. 32)
constexpr std::pair<std::uint32_t, std::uint8_t> kBauds[] = {
    {1200, 0}, {2400, 1}, {4800, 2}, {9600, 3}, {19200, 4}, {38400, 5}, {57600, 6}, {115200, 7}};
// REG0 [2:0] air rate: 000, 001 and 010 = 2.4k (010 is the default); we write 010
constexpr std::pair<std::uint32_t, std::uint8_t> kAirRates[] = {
    {2400, 2}, {4800, 3}, {9600, 4}, {19200, 5}, {38400, 6}, {62500, 7}};
// REG1 [1:0] power (E220-400T30S): 00=30, 01=27, 10=24, 11=21 dBm (6.3, p. 33)
constexpr std::pair<std::uint8_t, std::uint8_t> kPowers[] = {{30, 0}, {27, 1}, {24, 2}, {21, 3}};
// REG1 [7:6] packet length: 200/128/64/32 B (6.3, p. 32)
constexpr std::pair<std::uint16_t, std::uint8_t> kPackets[] = {{200, 0}, {128, 1}, {64, 2}, {32, 3}};

// bits 7:6 (packet length), 5 (ambient RSSI), 1:0 (power); bit 2 is unverified and left as read
constexpr std::uint8_t kReg1ManagedMask = 0xE3;
constexpr std::uint8_t kReg1AmbientRssi = 0x20;  // bit5
constexpr std::uint8_t kReg3RssiByte = 0x80;     // bit7
constexpr std::uint8_t kReg3Fixed = 0x40;        // bit6
constexpr std::uint8_t kReg3Lbt = 0x10;          // bit4

std::uint8_t NormalizeReg0(std::uint8_t v) {
  std::uint8_t parity = (v >> 3) & 0x3;
  if (parity == 3) {
    parity = 0;  // 11 = 8N1 (equivalent to 00)
  }
  std::uint8_t air = v & 0x7;
  if (air < 2) {
    air = 2;  // 000, 001, 010 = 2.4k
  }
  return static_cast<std::uint8_t>((v & 0xE0) | (parity << 3) | air);
}

template <typename K, std::size_t N>
std::optional<std::uint8_t> Lookup(const std::pair<K, std::uint8_t> (&table)[N], K key) {
  for (const auto& e : table) {
    if (e.first == key) {
      return e.second;
    }
  }
  return std::nullopt;
}

template <typename K, std::size_t N>
std::string AllowedList(const std::pair<K, std::uint8_t> (&table)[N]) {
  std::string s;
  for (const auto& e : table) {
    s += (s.empty() ? "" : ", ") + std::to_string(e.first);
  }
  return s;
}
}  // namespace

std::optional<std::uint8_t> BaudCode(std::uint32_t baud) { return Lookup(kBauds, baud); }
std::optional<std::uint8_t> AirRateCode(std::uint32_t bps) { return Lookup(kAirRates, bps); }
std::optional<std::uint8_t> PowerCode(std::uint8_t dbm) { return Lookup(kPowers, dbm); }
std::optional<std::uint8_t> PacketCode(std::uint16_t bytes) { return Lookup(kPackets, bytes); }

std::string Validate(const E220Config& c) {
  if (c.channel > 83) {
    return "channel=" + std::to_string(c.channel) + " out of range 0..83 (E220-400, 6.3 REG2)";
  }
  if (!AirRateCode(c.air_rate_bps)) {
    return "air_rate=" + std::to_string(c.air_rate_bps) + " not allowed (" + AllowedList(kAirRates) + ")";
  }
  if (!PowerCode(c.power_dbm)) {
    return "power_dbm=" + std::to_string(c.power_dbm) + " not allowed for T30S (" +
           AllowedList(kPowers) + ")";
  }
  if (!BaudCode(c.uart_baudrate)) {
    return "uart_baudrate=" + std::to_string(c.uart_baudrate) + " not allowed (" +
           AllowedList(kBauds) + ")";
  }
  if (!PacketCode(c.packet_length)) {
    return "packet_length=" + std::to_string(c.packet_length) + " not allowed (" +
           AllowedList(kPackets) + ")";
  }
  if (c.wor_cycle && *c.wor_cycle > 7) {
    return "wor_cycle=" + std::to_string(*c.wor_cycle) + " out of range 0..7";
  }
  return "";
}

ParseResult ParseConfigJson(const std::string& text) {
  ParseResult r;
  nlohmann::json j;
  try {
    j = nlohmann::json::parse(text);
  } catch (const std::exception& e) {
    r.error = std::string("invalid JSON: ") + e.what();
    return r;
  }
  if (j.is_object() && j.contains("e220") && j["e220"].is_object()) {
    j = j["e220"];
  }
  if (!j.is_object()) {
    r.error = "the configuration must be a JSON object";
    return r;
  }
  static const std::set<std::string> kKeys = {"address",   "channel",       "air_rate", "power_dbm",
                                              "uart_baudrate", "parity", "packet_length", "lbt",
                                              "wor_cycle"};
  for (const auto& kv : j.items()) {
    if (kKeys.count(kv.key()) == 0) {
      r.error = "unknown field '" + kv.key() + "'";
      return r;
    }
  }
  auto get_uint = [&](const char* key, std::uint64_t max, std::uint64_t* out) -> bool {
    if (!j.contains(key)) {
      return true;
    }
    const auto& v = j[key];
    if (!v.is_number_integer() || v.get<std::int64_t>() < 0 ||
        static_cast<std::uint64_t>(v.get<std::int64_t>()) > max) {
      r.error = std::string(key) + " must be an integer 0.." + std::to_string(max);
      return false;
    }
    *out = static_cast<std::uint64_t>(v.get<std::int64_t>());
    return true;
  };
  std::uint64_t v = 0;
  E220Config c;
  if (j.contains("address")) {
    if (!get_uint("address", 65535, &v)) return r;
    c.address = static_cast<std::uint16_t>(v);
  }
  if (j.contains("channel")) {
    if (!get_uint("channel", 255, &v)) return r;
    c.channel = static_cast<std::uint8_t>(v);
  }
  if (j.contains("air_rate")) {
    if (!get_uint("air_rate", 1000000, &v)) return r;
    c.air_rate_bps = static_cast<std::uint32_t>(v);
  }
  if (j.contains("power_dbm")) {
    if (!get_uint("power_dbm", 255, &v)) return r;
    c.power_dbm = static_cast<std::uint8_t>(v);
  }
  if (j.contains("uart_baudrate")) {
    if (!get_uint("uart_baudrate", 10000000, &v)) return r;
    c.uart_baudrate = static_cast<std::uint32_t>(v);
  }
  if (j.contains("packet_length")) {
    if (!get_uint("packet_length", 65535, &v)) return r;
    c.packet_length = static_cast<std::uint16_t>(v);
  }
  if (j.contains("wor_cycle")) {
    if (!get_uint("wor_cycle", 255, &v)) return r;
    c.wor_cycle = static_cast<std::uint8_t>(v);
  }
  if (j.contains("lbt")) {
    if (!j["lbt"].is_boolean()) {
      r.error = "lbt must be a boolean";
      return r;
    }
    c.lbt = j["lbt"].get<bool>();
  }
  if (j.contains("parity")) {
    if (!j["parity"].is_string()) {
      r.error = "parity must be a string (8N1, 8O1, 8E1)";
      return r;
    }
    const auto p = j["parity"].get<std::string>();
    if (p == "8N1") {
      c.parity = UartParity::k8N1;
    } else if (p == "8O1") {
      c.parity = UartParity::k8O1;
    } else if (p == "8E1") {
      c.parity = UartParity::k8E1;
    } else {
      r.error = "parity='" + p + "' not allowed (8N1, 8O1, 8E1)";
      return r;
    }
  }
  r.error = Validate(c);
  r.ok = r.error.empty();
  r.config = c;
  return r;
}

Registers DesiredRegisters(const E220Config& c, const Registers& current) {
  Registers d = current;
  d[kRegAddrH] = static_cast<std::uint8_t>(c.address >> 8);
  d[kRegAddrL] = static_cast<std::uint8_t>(c.address & 0xFF);
  d[kReg0] = static_cast<std::uint8_t>((*BaudCode(c.uart_baudrate) << 5) |
                                       (static_cast<std::uint8_t>(c.parity) << 3) |
                                       *AirRateCode(c.air_rate_bps));
  // REG1: packet length, ambient RSSI (off unless the statistics are on), power; bits 4:3 (reserved)
  // and bit 2 stay as read
  d[kReg1] = static_cast<std::uint8_t>((current[kReg1] & ~kReg1ManagedMask) |
                                       (*PacketCode(c.packet_length) << 6) | *PowerCode(c.power_dbm) |
                                       (c.ambient_rssi ? kReg1AmbientRssi : 0));
  d[kReg2] = c.channel;
  // REG3: RSSI byte = 0, transparent transmission (fixed = 0), LBT from the configuration
  std::uint8_t r3 = current[kReg3] & static_cast<std::uint8_t>(~(kReg3RssiByte | kReg3Fixed | kReg3Lbt));
  if (c.lbt) {
    r3 |= kReg3Lbt;
  }
  if (c.wor_cycle) {
    r3 = static_cast<std::uint8_t>((r3 & ~0x07) | *c.wor_cycle);
  }
  d[kReg3] = r3;
  return d;
}

bool RegisterMatches(std::size_t reg, std::uint8_t actual, std::uint8_t desired, const E220Config& c) {
  switch (reg) {
    case kReg0:
      return NormalizeReg0(actual) == NormalizeReg0(desired);
    case kReg1:
      return (actual & kReg1ManagedMask) == (desired & kReg1ManagedMask);
    case kReg3: {
      std::uint8_t mask = kReg3RssiByte | kReg3Fixed | kReg3Lbt;
      if (c.wor_cycle) {
        mask |= 0x07;
      }
      return (actual & mask) == (desired & mask);
    }
    default:
      return actual == desired;
  }
}

std::vector<std::size_t> DifferingRegisters(const Registers& actual, const Registers& desired,
                                            const E220Config& c) {
  std::vector<std::size_t> out;
  for (std::size_t i = 0; i < kRegisterCount; ++i) {
    if (!RegisterMatches(i, actual[i], desired[i], c)) {
      out.push_back(i);
    }
  }
  return out;
}

std::string DescribeRegisters(const Registers& r) {
  static const std::uint32_t bauds[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
  static const std::uint32_t air[] = {2400, 2400, 2400, 4800, 9600, 19200, 38400, 62500};
  static const char* parity[] = {"8N1", "8O1", "8E1", "8N1"};
  static const int power[] = {30, 27, 24, 21};
  static const int packet[] = {200, 128, 64, 32};
  char buf[256];
  std::snprintf(buf, sizeof(buf),
                "addr=0x%02X%02X uart=%u %s air=%u pkt=%dB power=%ddBm ch=%u reg3=0x%02X(%s%s%s)",
                r[kRegAddrH], r[kRegAddrL], bauds[r[kReg0] >> 5], parity[(r[kReg0] >> 3) & 3],
                air[r[kReg0] & 7], packet[r[kReg1] >> 6], power[r[kReg1] & 3], r[kReg2], r[kReg3],
                (r[kReg3] & kReg3Fixed) ? "fixed" : "transparent",
                (r[kReg3] & kReg3RssiByte) ? ",rssi-byte" : "", (r[kReg3] & kReg3Lbt) ? ",lbt" : "");
  return buf;
}

namespace {
DriverConfigResult Fail(std::string msg) {
  DriverConfigResult r;
  r.error = std::move(msg);
  return r;
}

bool PinNumber(const nlohmann::json& pins, const char* key, std::uint16_t* out, std::string* err) {
  if (!pins.contains(key)) {
    *err = std::string("pins: missing field '") + key + "'";
    return false;
  }
  const auto& v = pins[key];
  if (!v.is_number_integer() || v.get<std::int64_t>() < 0 || v.get<std::int64_t>() > 65535) {
    *err = std::string("pins.") + key + " must be a GPIO number (integer 0..65535)";
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
    return Fail(std::string("invalid JSON: ") + e.what());
  }
  if (!j.is_object()) {
    return Fail("the driver configuration must be a JSON object");
  }
  static const std::set<std::string> kKeys = {"uart", "pins", "aux_idle_level", "e220", "stats_period_ms"};
  for (const auto& kv : j.items()) {
    if (kKeys.count(kv.key()) == 0) {
      return Fail("unknown field '" + kv.key() + "' (allowed: uart, pins, aux_idle_level, e220, stats_period_ms)");
    }
  }
  DriverConfigResult r;
  if (!j.contains("uart") || !j["uart"].is_string() || j["uart"].get<std::string>().empty()) {
    return Fail("missing field 'uart' (UART device path, e.g. \"/dev/ttyS2\")");
  }
  r.config.uart = j["uart"].get<std::string>();
  if (!j.contains("pins") || !j["pins"].is_object()) {
    return Fail("missing object 'pins' ({\"m0\": N, \"m1\": N, \"aux\": N} - sysfs GPIO numbers)");
  }
  const auto& pins = j["pins"];
  static const std::set<std::string> kPinKeys = {"m0", "m1", "aux", "ctl_dir"};
  for (const auto& kv : pins.items()) {
    if (kPinKeys.count(kv.key()) == 0) {
      return Fail("pins: unknown field '" + kv.key() + "' (allowed: m0, m1, aux)");
    }
  }
  std::string err;
  if (pins.contains("ctl_dir")) {
    if (!pins["ctl_dir"].is_string() || pins["ctl_dir"].get<std::string>().empty()) {
      return Fail("pins.ctl_dir must be a non-empty string");
    }
    r.config.pins.ctl_dir = pins["ctl_dir"].get<std::string>();
  } else {
    if (!PinNumber(pins, "m0", &r.config.pins.m0, &err) || !PinNumber(pins, "m1", &r.config.pins.m1, &err) ||
        !PinNumber(pins, "aux", &r.config.pins.aux, &err)) {
      return Fail(err);
    }
    if (r.config.pins.m0 == r.config.pins.m1 || r.config.pins.m0 == r.config.pins.aux ||
        r.config.pins.m1 == r.config.pins.aux) {
      return Fail("pins: m0, m1 and aux must be different GPIO numbers");
    }
  }
  if (j.contains("e220")) {
    if (!j["e220"].is_object()) {
      return Fail("field 'e220' must be an object (radio parameters)");
    }
    const auto radio = ParseConfigJson(j["e220"].dump());
    if (!radio.ok) {
      return Fail("e220: " + radio.error);
    }
    r.config.radio = radio.config;
  }
  if (r.config.radio.parity != UartParity::k8N1) {
    return Fail("e220.parity: only \"8N1\" is supported (the UART adapter has no parity support)");
  }
  if (j.contains("aux_idle_level")) {
    const auto& v = j["aux_idle_level"];
    if (!v.is_number_integer() || (v.get<std::int64_t>() != 0 && v.get<std::int64_t>() != 1)) {
      return Fail("aux_idle_level must be 1 or 0 (AUX level while the module is idle)");
    }
    r.config.radio.aux_idle_level = v.get<std::int64_t>() == 1;
  }
  if (j.contains("stats_period_ms")) {
    const auto& v = j["stats_period_ms"];
    if (!v.is_number_integer() || v.get<std::int64_t>() < 0 || v.get<std::int64_t>() > 3600000) {
      return Fail("stats_period_ms must be an integer 0..3600000 (RSSI/noise readout period in ms, 0 = off)");
    }
    r.config.stats_period_ms = static_cast<std::uint32_t>(v.get<std::int64_t>());
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
    return Fail("cannot read the configuration file '" + path + "'");
  }
  std::stringstream ss;
  ss << in.rdbuf();
  auto r = ParseDriverConfigJson(ss.str());
  if (!r.ok) {
    r.error = "file '" + path + "': " + r.error;
  }
  return r;
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
