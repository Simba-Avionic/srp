/**
 * @file sim_i2c_driver.cpp
 * @brief Sim I2C: model magistrali per adres, sterowany z zewnatrz (core/sim/sim_ctl.hpp).
 *
 * Urzadzenia: 0x50 EEPROM 24LC32 (4 KiB, adres 16-bit), 0x48 ADS7828, 0x70 PCA9685,
 * 0x6A LSM6DSOX, 0x76 BME280. Semantyka Write(vector) jak w prawdziwym driverze
 * (core/i2c/i2c_driver.cpp): pary {reg,wartosc}, dlugosc nieparzysta/pusta = blad.
 * Polecenia ctl i2c: patrz core/sim/README.md.
 *
 * @copyright Copyright (c) 2025
 */
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <map>
#include <mutex>  // NOLINT
#include <thread>  // NOLINT

#include "ara/log/log.h"
#include "core/i2c/i2c_driver.hpp"
#include "core/sim/sim_ctl.hpp"

namespace srp {
namespace core {
namespace i2c {

namespace {

enum class Kind { kEeprom, kAds, kRegs };
enum class Fault { kNone, kNack, kTimeout };

struct Device {
  Kind kind{Kind::kRegs};
  std::vector<uint8_t> mem;
  size_t ptr{0};
  uint8_t ads_sel{0};
};

int ParseInt(const std::string& s) { return static_cast<int>(sim::ParseNum(s, 0)); }

class Bus {
 public:
  static Bus& Get() {
    static Bus b;
    return b;
  }

  Fault Check(uint8_t addr) {
    std::lock_guard<std::mutex> l(m_);
    return Sync(addr);
  }

  bool Txn(uint8_t addr, const std::vector<uint8_t>& bytes) {
    std::lock_guard<std::mutex> l(m_);
    auto it = devs_.find(addr);
    if (it == devs_.end() || bytes.empty()) {
      return false;
    }
    Device& d = it->second;
    if (d.kind == Kind::kAds) {
      d.ads_sel = (bytes[0] >> 4) & 7;
    } else if (d.kind == Kind::kEeprom) {
      if (bytes.size() == 1) {
        d.ptr = (static_cast<size_t>(bytes[0] & 0x0F) << 8);
      } else {
        d.ptr = ((static_cast<size_t>(bytes[0]) << 8) | bytes[1]) & 0x0FFF;
        for (size_t i = 2; i < bytes.size(); ++i) {
          d.mem[d.ptr] = bytes[i];
          d.ptr = (d.ptr + 1) & 0x0FFF;
        }
      }
    } else {
      d.ptr = bytes[0];
      for (size_t i = 1; i < bytes.size(); ++i) {
        if (!(addr == 0x6A && d.ptr == 0x0F)) {  // WHO_AM_I tylko do odczytu
          d.mem[d.ptr & 0xFF] = bytes[i];
        }
        d.ptr = (d.ptr + 1) & 0xFF;
      }
    }
    if (bytes.size() > 1 || d.kind != Kind::kRegs) {
      sim::Event("i2c", "\"op\":\"write\",\"addr\":" + std::to_string(addr) +
                            ",\"data\":\"" + sim::Hex(bytes) + "\"");
    }
    if (d.kind != Kind::kAds && bytes.size() > 1) {
      Dump(addr);
    }
    return true;
  }

  std::optional<std::vector<uint8_t>> Read(uint8_t addr, size_t size) {
    std::lock_guard<std::mutex> l(m_);
    auto it = devs_.find(addr);
    if (it == devs_.end()) {
      return std::nullopt;
    }
    if (size == 0) {
      return std::vector<uint8_t>{};  // like read(fd, buf, 0) == 0 on a real device
    }
    Device& d = it->second;
    std::vector<uint8_t> out;
    if (d.kind == Kind::kAds) {
      // odwrotnosc channelMap z mw/i2c_service/controller/ads7828: sel -> kanal
      static const int kSelToCh[8] = {0, 2, 4, 6, 1, 3, 5, 7};
      const double v = volts_[kSelToCh[d.ads_sel & 7]];
      const double scaled = v / 3.3 * 4096.0 + 0.5;
      const long raw = scaled < 0.0 ? 0 : (scaled > 4095.0 ? 4095 : static_cast<long>(scaled));  // NOLINT
      out = {static_cast<uint8_t>(raw >> 8), static_cast<uint8_t>(raw & 0xFF)};
      out.resize(size, 0);
      return out;
    }
    const size_t mask = (d.kind == Kind::kEeprom) ? 0x0FFF : 0xFF;
    for (size_t i = 0; i < size; ++i) {
      out.push_back(d.mem[d.ptr & mask]);
      d.ptr = (d.ptr + 1) & mask;
    }
    return out;
  }

 private:
  Bus() { Reset(); }

  void Reset() {
    devs_.clear();
    for (auto& v : volts_) {
      v = 0.0;
    }
    Device e;
    e.kind = Kind::kEeprom;
    e.mem.assign(4096, 0xFF);
    WriteCfg(&e, 1.0f, {"0b2554dfd2cc", "0b25550c2889", "0b2554e84c21"});
    devs_[0x50] = e;
    Device a;
    a.kind = Kind::kAds;
    devs_[0x48] = a;
    Device p;
    p.mem.assign(256, 0);
    devs_[0x70] = p;
    Device g;
    g.mem.assign(256, 0);
    g.mem[0x0F] = 0x6C;  // WHO_AM_I
    g.mem[0x1E] = 0x07;  // STATUS: dane gotowe
    devs_[0x6A] = g;
    Device b;
    b.mem.assign(256, 0);
    b.mem[0xD0] = 0x60;  // chip id BME280
    devs_[0x76] = b;
  }

  // Uklad EEPROM_config (mw/i2c_service/controller/24lc32at/eeprom_config/cfg_manager.hpp):
  // float xo (4 B) + 3 x char[13] (surowe id czujnikow bez prefiksu "28-").
  static void WriteCfg(Device* e, float xo, const std::vector<std::string>& ids) {
    std::memcpy(e->mem.data(), &xo, sizeof(xo));
    for (size_t i = 0; i < 3 && i < ids.size(); ++i) {
      uint8_t* p = e->mem.data() + 4 + 13 * i;
      std::memset(p, 0, 13);
      std::memcpy(p, ids[i].data(), std::min<size_t>(ids[i].size(), 13));
    }
  }

  void Dump(uint8_t addr) {
    char n[16];
    std::snprintf(n, sizeof(n), "0x%02x", addr);
    sim::WriteState(std::string("i2c/") + n + ".hex", sim::Hex(devs_[addr].mem));
  }

  // Stosuje polecenia ctl (settery jednorazowo) i zwraca usterke dla adresu.
  Fault Sync(uint8_t addr) {
    Fault f = Fault::kNone;
    sim::Ctl::Get().Visit([&](sim::Cmd& c) {
      if (c.tok.size() < 2 || c.tok[0] != "i2c") {
        return;
      }
      try {
        const std::string& op = c.tok[1];
        if (op == "fault" && c.tok.size() >= 4) {
          if (ParseInt(c.tok[2]) == addr && f == Fault::kNone && sim::Consume(&c)) {
            f = (c.tok[3] == "timeout") ? Fault::kTimeout : Fault::kNack;
            sim::Event("i2c", "\"op\":\"fault\",\"addr\":" + std::to_string(addr) +
                                  ",\"kind\":\"" + c.tok[3] + "\"");
          }
          return;
        }
        if (c.applied) {
          return;
        }
        c.applied = true;
        if (op == "reset") {
          Reset();
        } else if (op == "adc" && c.tok.size() >= 4) {
          volts_[ParseInt(c.tok[2]) & 7] = std::stod(c.tok[3]);
        } else if (op == "mem" && c.tok.size() >= 5) {
          const uint8_t a = static_cast<uint8_t>(ParseInt(c.tok[2]));
          const size_t off = ParseInt(c.tok[3]);
          auto bytes = sim::UnHex(c.tok, 4);
          auto it = devs_.find(a);
          if (it != devs_.end()) {
            for (size_t i = 0; i < bytes.size() && off + i < it->second.mem.size(); ++i) {
              it->second.mem[off + i] = bytes[i];
            }
            Dump(a);
          }
        } else if (op == "eeprom-cfg" && c.tok.size() >= 6) {
          const float xo = (c.tok[2] == "nan") ? std::numeric_limits<float>::quiet_NaN() : std::stof(c.tok[2]);
          WriteCfg(&devs_[0x50], xo, {c.tok[3], c.tok[4], c.tok[5]});
          Dump(0x50);
        }
      } catch (...) {
        ara::log::LogWarn() << "sim i2c: bad ctl line: " << c.text;
      }
    });
    return f;
  }

  std::mutex m_;
  std::map<uint8_t, Device> devs_;
  double volts_[8]{};
};

core::ErrorCode FaultToError(Fault f, core::ErrorCode code = core::ErrorCode::kConnectionError) {
  if (f == Fault::kTimeout) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  }
  return code;
}

}  // namespace

I2CDriver::I2CDriver(): i2c_logger_ {
  ara::log::LoggingMenager::GetInstance()->CreateLogger("I2C-",
  "", ara::log::LogLevel::kWarn)} {
}

I2CDriver::~I2CDriver() {}

core::ErrorCode I2CDriver::Init() {
  sim::MkDirs(sim::Dir());
  return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Ioctl(const uint8_t address, const uint16_t type) {
  current_address_ = address;
  return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Write(const std::vector<uint8_t>& RegData) {
  if (RegData.empty() || (RegData.size() % 2) != 0) {
    return core::ErrorCode::kInitializeError;
  }
  Fault f = Bus::Get().Check(current_address_);
  if (f != Fault::kNone) {
    return FaultToError(f, core::ErrorCode::kInitializeError);  // as i2c_driver.cpp:55-56
  }
  for (std::size_t i = 0; i < RegData.size(); i += 2) {
    if (!Bus::Get().Txn(current_address_, {RegData[i], RegData[i + 1]})) {
      return core::ErrorCode::kInitializeError;
    }
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode I2CDriver::Write(const uint8_t& data) {
  Fault f = Bus::Get().Check(current_address_);
  if (f != Fault::kNone) {
    return FaultToError(f);
  }
  return Bus::Get().Txn(current_address_, {data}) ? core::ErrorCode::kOk
                                                   : core::ErrorCode::kConnectionError;
}

std::optional<std::vector<uint8_t>> I2CDriver::Read(const uint8_t size) {
  Fault f = Bus::Get().Check(current_address_);
  if (f != Fault::kNone) {
    FaultToError(f);
    return std::nullopt;
  }
  return Bus::Get().Read(current_address_, size);
}

core::ErrorCode I2CDriver::PageWrite(std::vector<uint8_t> data) {
  Fault f = Bus::Get().Check(current_address_);
  if (f != Fault::kNone) {
    return FaultToError(f, core::ErrorCode::kInitializeError);  // as i2c_driver.cpp:76
  }
  return Bus::Get().Txn(current_address_, data) ? core::ErrorCode::kOk
                                                 : core::ErrorCode::kInitializeError;
}

std::optional<std::vector<uint8_t>> I2CDriver::ReadWrite(const uint8_t& reg, const uint8_t size) {
  Fault f = Bus::Get().Check(current_address_);
  if (f != Fault::kNone) {
    FaultToError(f);
    return std::nullopt;
  }
  if (!Bus::Get().Txn(current_address_, {reg})) {
    return std::nullopt;
  }
  return Bus::Get().Read(current_address_, size);
}

std::optional<std::vector<uint8_t>> I2CDriver::ReadWrite(
    const std::vector<uint8_t>& write_data, const uint8_t size) {
  if (write_data.empty() || size == 0) {
    return std::nullopt;
  }
  Fault f = Bus::Get().Check(current_address_);
  if (f != Fault::kNone) {
    FaultToError(f);
    return std::nullopt;
  }
  if (!Bus::Get().Txn(current_address_, write_data)) {
    return std::nullopt;
  }
  return Bus::Get().Read(current_address_, size);
}

}  // namespace i2c
}  // namespace core
}  // namespace srp
