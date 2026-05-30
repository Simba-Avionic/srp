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
#include <thread>  // NOLINT
#include <atomic>
#include <chrono>  // NOLINT
#include <mutex>  // NOLINT

#include "ara/log/log.h"


namespace srp {
namespace core {
namespace stat {

namespace {
    static constexpr auto kMemInfoPath = "/proc/meminfo";
    static constexpr std::chrono::milliseconds kCpuSampleInterval{100};
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
        if (line.find("MemTotal:") == 0) {
            sscanf(line.c_str(), "MemTotal: %ld kB", &total);
        } else if (line.find("MemAvailable:") == 0) {
            sscanf(line.c_str(), "MemAvailable: %ld kB", &available);
        }
    }
    if (total <= 0) return std::nullopt;

    return 100.0f * static_cast<float>(total - available) / static_cast<float>(total);
}

struct CPUState {
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
};

// Funkcja pomocnicza do czytania /proc/stat
std::optional<CPUState> read_cpu_state() {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) return std::nullopt;

    std::string cpu;
    CPUState state;
    // Interesuje nas pierwsza linijka podsumowująca cały procesor
    if (file >> cpu >> state.user >> state.nice >> state.system >> state.idle
             >> state.iowait >> state.irq >> state.softirq >> state.steal) {
        return state;
    }
    return std::nullopt;
}

namespace {
std::atomic<bool> g_cpu_sampler_started{false};
std::atomic<bool> g_has_cpu_sample{false};
std::atomic<double> g_last_cpu_usage{0.0};
std::once_flag g_cpu_sampler_once;

double calculate_cpu_usage(const CPUState& prev, const CPUState& curr) {
    auto prev_idle = prev.idle + prev.iowait;
    auto curr_idle = curr.idle + curr.iowait;

    auto prev_total =
        prev.user + prev.nice + prev.system + prev.idle + prev.iowait + prev.irq + prev.softirq +
        prev.steal;
    auto curr_total =
        curr.user + curr.nice + curr.system + curr.idle + curr.iowait + curr.irq + curr.softirq +
        curr.steal;

    auto total_delta = curr_total - prev_total;
    auto idle_delta = curr_idle - prev_idle;
    if (total_delta == 0) {
        return 0.0;
    }

    return 100.0 * static_cast<double>(total_delta - idle_delta) / static_cast<double>(total_delta);
}

void start_cpu_sampler_thread() {
    std::call_once(g_cpu_sampler_once, [] {
        std::thread([] {
            auto prev = read_cpu_state();
            if (!prev) {
                return;
            }

            g_cpu_sampler_started.store(true, std::memory_order_release);

            while (true) {
                std::this_thread::sleep_for(kCpuSampleInterval);
                auto curr = read_cpu_state();
                if (!curr) {
                    continue;
                }

                g_last_cpu_usage.store(calculate_cpu_usage(*prev, *curr), std::memory_order_release);
                g_has_cpu_sample.store(true, std::memory_order_release);
                prev = curr;
            }
        }).detach();
    });
}
}  // namespace

std::optional<double> SystemStats::get_cpu_usage() {
    start_cpu_sampler_thread();
    if (!g_cpu_sampler_started.load(std::memory_order_acquire) ||
        !g_has_cpu_sample.load(std::memory_order_acquire)) {
        return std::nullopt;
    }

    return g_last_cpu_usage.load(std::memory_order_acquire);
}

}  // namespace stat
}  // namespace core
}  // namespace srp
