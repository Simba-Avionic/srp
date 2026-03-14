/**
 * @file system_stat.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-02-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "core/sys/system_stat.hpp"
#include <stdlib.h>
#include <cstdint>
#include <fstream>
#include <cstdio>
#include <memory>
#include <optional>
#include <array>
#include <string>
#include <filesystem>
#include <cmath>
#include "ara/log/log.h"

namespace srp {
namespace core {
namespace stat {

namespace {
    constexpr auto kMemInfoPath = "/proc/meminfo";
}

namespace fs = std::filesystem;

double SystemStats::get_disk_space() {
    fs::space_info si = fs::space("/");
    double usage = 100.0 * (si.capacity - si.available) / si.capacity;
    usage = usage;
    return usage;
}

std::optional<float> SystemStats::get_ram_usage() {
    std::ifstream file(kMemInfoPath);
    std::string line;
    int64_t total = 0, available = 0;

    while (std::getline(file, line)) {
        if (line.compare(0, 8, "MemTotal") == 0) {
            sscanf(line.c_str(), "MemTotal: %ld kB", &total);
        } else if (line.compare(0, 12, "MemAvailable") == 0) {
            sscanf(line.c_str(), "MemAvailable: %ld kB", &available);
        }
        if (total == 0 && available == 0) {
            return std::nullopt;
        }
    }
    float ram_usage = 100.0f * static_cast<float>(total - available) / static_cast<float>(total);
    ram_usage = static_cast<float>(ram_usage);
    return ram_usage;
}

std::optional<double> SystemStats::get_cpu_usage() {
    double load[1];
    if (getloadavg(load, 1) <= 0) {
        return std::nullopt;
    }
    return load[0] * 25;
}

}  // namespace stat
}  // namespace core
}  // namespace srp
