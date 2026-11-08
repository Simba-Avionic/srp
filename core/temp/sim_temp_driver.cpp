/**
 * @file sim_temp_driver.cpp
 * @brief Sim czujnikow 1-Wire (DS18B20) sterowany tabela id->wartosc/blad (core/sim/sim_ctl.hpp).
 *
 * Jak prawdziwy driver (core/temp/temp_driver.cpp) zwraca wartosc czujnika bez
 * walidacji (m.in. 85.0 / -127.0). Polecenia ctl temp: docs/audit/sim_drivers.md.
 *
 * @copyright Copyright (c) 2025
 */
#include <chrono>  // NOLINT
#include <cstdint>
#include <map>
#include <mutex>  // NOLINT
#include <optional>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/com/com_error_domain.h"
#include "ara/core/result.h"
#include "ara/log/log.h"
#include "core/common/condition.h"
#include "core/sim/sim_ctl.hpp"
#include "core/temp/temp_driver.hpp"

namespace srp {
namespace core {
namespace temp {
namespace {

enum class Mode { kValue, kErr, kTimeout };
struct Sensor {
    Mode mode{Mode::kValue};
    double value{25.0};
    uint8_t resolution{9};
    uint16_t conv_ms{250};
};

std::mutex g_m;
std::map<std::string, Sensor> g_sensors;  // klucz: pelne physical id, np. "28-0b2554dfd2cc"
bool g_init = false;

void Defaults() {
    g_sensors.clear();
    for (const char* id : {"28-0b2554dfd2cc", "28-0b25550c2889", "28-0b2554e84c21"}) {
        g_sensors[id] = Sensor{};
    }
}

void Sync() {
    if (!g_init) {
        Defaults();
        g_init = true;
    }
    sim::Ctl::Get().Visit([&](sim::Cmd& c) {
        if (c.tok.size() < 2 || c.tok[0] != "temp" || c.applied) {
            return;
        }
        c.applied = true;
        try {
            const std::string& k = c.tok[1];
            if (k == "reset") {
                Defaults();
            } else if (k == "list") {  // temp list <id>...  (zastepuje liste czujnikow)
                std::map<std::string, Sensor> n;
                for (size_t i = 2; i < c.tok.size(); ++i) {
                    n[c.tok[i]] = g_sensors.count(c.tok[i]) ? g_sensors[c.tok[i]] : Sensor{};
                }
                g_sensors = n;
            } else if (k == "remove" && c.tok.size() >= 3) {
                g_sensors.erase(c.tok[2]);
            } else if (c.tok.size() >= 3) {  // temp <id> <wartosc|err|timeout>
                Sensor& s = g_sensors[k];
                if (c.tok[2] == "err") {
                    s.mode = Mode::kErr;
                } else if (c.tok[2] == "timeout") {
                    s.mode = Mode::kTimeout;
                } else {
                    s.mode = Mode::kValue;
                    s.value = std::stod(c.tok[2]);
                }
                sim::Event("temp", "\"op\":\"set\",\"id\":\"" + k + "\",\"val\":\"" + c.tok[2] + "\"");
            }
        } catch (...) {
            ara::log::LogWarn() << "sim temp: bad ctl line: " << c.text;
        }
    });
}

auto Missing(const char* what) {
    return ara::com::MakeErrorCode(ara::com::ComErrc::kErroneousFileHandle, what);
}
}  // namespace

TempDriver::TempDriver(): logger_(ara::log::LoggingMenager::GetInstance()
        ->CreateLogger("Dtem", "", ara::log::LogLevel::kWarn)) {
}

ara::core::Result<double> TempDriver::ReadTemp(const std::string& sensorPhysicalID) {
    Sensor s;
    {
        std::lock_guard<std::mutex> l(g_m);
        Sync();
        auto it = g_sensors.find(sensorPhysicalID);
        if (it == g_sensors.end()) {
            return Missing("Can't open file for 1Wire sensor");
        }
        s = it->second;
    }
    if (s.mode == Mode::kTimeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return Missing("Sensor timeout");
    }
    if (s.mode == Mode::kErr) {
        return Missing("Parsing error");
    }
    return s.value;
}

ara::core::Result<bool> TempDriver::SetResolution(const std::string& sensorPhysicalID, uint8_t resolution) {
    std::lock_guard<std::mutex> l(g_m);
    Sync();
    auto it = g_sensors.find(sensorPhysicalID);
    if (it == g_sensors.end()) {
        return Missing("Can't open file for 1Wire sensor");
    }
    it->second.resolution = resolution;
    return true;
}

ara::core::Result<uint8_t> TempDriver::ReadResolution(const std::string& sensorPhysicalID) {
    std::lock_guard<std::mutex> l(g_m);
    Sync();
    auto it = g_sensors.find(sensorPhysicalID);
    if (it == g_sensors.end()) {
        return Missing("Can't open file for 1Wire sensor");
    }
    return it->second.resolution;
}

ara::core::Result<std::vector<std::string>> TempDriver::GetAllAccessibleSensor() {
    std::lock_guard<std::mutex> l(g_m);
    Sync();
    std::vector<std::string> res;
    for (const auto& kv : g_sensors) {
        res.push_back(kv.first);
    }
    return res;
}

ara::core::Result<uint16_t> TempDriver::GetResponseTime(const std::string& sensorPhysicalID) {
    std::lock_guard<std::mutex> l(g_m);
    Sync();
    auto it = g_sensors.find(sensorPhysicalID);
    if (it == g_sensors.end()) {
        return Missing("Can't open file for 1Wire sensor");
    }
    return it->second.conv_ms;
}

}  // namespace temp
}  // namespace core
}  // namespace srp
