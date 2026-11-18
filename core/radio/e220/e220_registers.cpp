/**
 * Copyright (c) 2024
 *
 * @file e220_registers.cpp
 */
#include "core/radio/e220/e220_registers.hpp"

#include <algorithm>
#include <cstdio>
#include <set>
#include <sstream>
#include <utility>

#include "nlohmann/json.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {

// REG0 [7:5] baud (6.3, str. 32)
constexpr std::pair<std::uint32_t, std::uint8_t> kBauds[] = {
    {1200, 0}, {2400, 1}, {4800, 2}, {9600, 3}, {19200, 4}, {38400, 5}, {57600, 6}, {115200, 7}};
// REG0 [2:0] szybkosc radiowa: 000, 001 i 010 = 2.4k (010 domyslne); wybieramy 010
constexpr std::pair<std::uint32_t, std::uint8_t> kAirRates[] = {
    {2400, 2}, {4800, 3}, {9600, 4}, {19200, 5}, {38400, 6}, {62500, 7}};
// REG1 [1:0] moc (E220-400T30S): 00=30, 01=27, 10=24, 11=21 dBm (6.3, str. 33)
constexpr std::pair<std::uint8_t, std::uint8_t> kPowers[] = {{30, 0}, {27, 1}, {24, 2}, {21, 3}};
// REG1 [7:6] dlugosc pakietu: 200/128/64/32 B (6.3, str. 32)
constexpr std::pair<std::uint16_t, std::uint8_t> kPackets[] = {{200, 0}, {128, 1}, {64, 2}, {32, 3}};

constexpr std::uint8_t kReg1ManagedMask = 0xE7;  // bity 7,6 (pakiet), 5 (RSSI otoczenia), 2 (soft-switch), 1:0 (moc)
constexpr std::uint8_t kReg3RssiByte = 0x80;     // bit7
constexpr std::uint8_t kReg3Fixed = 0x40;        // bit6
constexpr std::uint8_t kReg3Lbt = 0x10;          // bit4

std::uint8_t NormalizeReg0(std::uint8_t v) {
  std::uint8_t parity = (v >> 3) & 0x3;
  if (parity == 3) {
    parity = 0;  // 11 = 8N1 (rownowazne 00)
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
    return "channel=" + std::to_string(c.channel) + " poza zakresem 0..83 (E220-400, 6.3 REG2)";
  }
  if (!AirRateCode(c.air_rate_bps)) {
    return "air_rate=" + std::to_string(c.air_rate_bps) + " niedozwolone (" + AllowedList(kAirRates) + ")";
  }
  if (!PowerCode(c.power_dbm)) {
    return "power_dbm=" + std::to_string(c.power_dbm) + " niedozwolone dla T30S (" +
           AllowedList(kPowers) + ")";
  }
  if (!BaudCode(c.uart_baudrate)) {
    return "uart_baudrate=" + std::to_string(c.uart_baudrate) + " niedozwolone (" +
           AllowedList(kBauds) + ")";
  }
  if (!PacketCode(c.packet_length)) {
    return "packet_length=" + std::to_string(c.packet_length) + " niedozwolone (" +
           AllowedList(kPackets) + ")";
  }
  if (c.wor_cycle && *c.wor_cycle > 7) {
    return "wor_cycle=" + std::to_string(*c.wor_cycle) + " poza zakresem 0..7";
  }
  return "";
}

ParseResult ParseConfigJson(const std::string& text) {
  ParseResult r;
  nlohmann::json j;
  try {
    j = nlohmann::json::parse(text);
  } catch (const std::exception& e) {
    r.error = std::string("niepoprawny JSON: ") + e.what();
    return r;
  }
  if (j.is_object() && j.contains("e220") && j["e220"].is_object()) {
    j = j["e220"];
  }
  if (!j.is_object()) {
    r.error = "konfiguracja musi byc obiektem JSON";
    return r;
  }
  static const std::set<std::string> kKeys = {"address",   "channel",       "air_rate", "power_dbm",
                                              "uart_baudrate", "parity", "packet_length", "lbt",
                                              "wor_cycle"};
  for (const auto& kv : j.items()) {
    if (kKeys.count(kv.key()) == 0) {
      r.error = "nieznane pole '" + kv.key() + "'";
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
      r.error = std::string(key) + " musi byc liczba calkowita 0.." + std::to_string(max);
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
      r.error = "lbt musi byc bool";
      return r;
    }
    c.lbt = j["lbt"].get<bool>();
  }
  if (j.contains("parity")) {
    if (!j["parity"].is_string()) {
      r.error = "parity musi byc stringiem (8N1, 8O1, 8E1)";
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
      r.error = "parity='" + p + "' niedozwolone (8N1, 8O1, 8E1)";
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
  // REG1: pakiet, RSSI otoczenia = 0, soft-switch = 0, moc; bity 4:3 (zarezerwowane) bez zmian
  d[kReg1] = static_cast<std::uint8_t>((current[kReg1] & ~kReg1ManagedMask) |
                                       (*PacketCode(c.packet_length) << 6) | *PowerCode(c.power_dbm));
  d[kReg2] = c.channel;
  // REG3: bajt RSSI = 0, transmisja transparentna (fixed = 0), LBT wg konfiguracji
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

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
