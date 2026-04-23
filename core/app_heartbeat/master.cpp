/**
 * @file master.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "core/app_heartbeat/master.hpp"
#include "core/common/condition.h"
#include "core/app_heartbeat/data_structure.h/srp/core/hb/HBHeader.h"
#include "ara/log/log.h"
#include "core/app_heartbeat/common.hpp"
namespace srp {
namespace core {
namespace hb {

namespace {
    static constexpr auto kRx_sock_name = "HB_SOCK";
    static constexpr auto kHeartbeat_tnterval_ms = 1000;
    static constexpr auto kHeartbeat_accepted_deviation_ms = 250;
}

void HeartBeatMaster::Loop(const std::stop_token& t) {
    while(!t.stop_requested()) {
        auto now = std::chrono::high_resolution_clock::now();
        {
            std::lock_guard<std::mutex> lock(mtx_);
            for (auto& app: apps) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - app.second.last_hb).count();
                if (duration > (kHeartbeat_tnterval_ms + kHeartbeat_accepted_deviation_ms)) {
                    app.second.active = false;
                }
            }
        }
        core::condition::wait_for(std::chrono::milliseconds(100), t);
    }
}

uint16_t HeartBeatMaster::GetAppsAlive() {
    std::lock_guard<std::mutex> lock(mtx_);
    uint16_t apps_alive;
    for (const auto& app : apps) {
        if (app.second.active) {
            apps_alive |= (1 << (app.first - 1));
        }
    }
    return apps_alive;
}

bool HeartBeatMaster::CheckAppAlive(const SERVICES_e& app) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = apps.find(app);
    if (it == apps.end()) {
        return false;
    }
    return it->second.active;
}

void HeartBeatMaster::RxCallback(const std::string& ip, const std::uint16_t& port,
                       std::vector<std::uint8_t> data) {
    auto hdr_opt = srp::data::Convert<HBHeader>::Conv(data);
    if (!hdr_opt.has_value()) {
        return;
    }
    bool update_req = false;

    auto service_id = hdr_opt.value().service_id;
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = apps.find(service_id);
    if (it != apps.end()) {
        it->second.last_hb = std::chrono::high_resolution_clock::now();
        if (!it->second.active) {
            update_req = true;
        }
    } else {
        apps.emplace(std::pair<uint16_t, app_t>(service_id, {std::chrono::high_resolution_clock::now(), true}));
        update_req = true;
    }
    if (update_req) {
        callback_(GetAppsAlive());
    }
}

void HeartBeatMaster::Init(onUpdate callback) {
    callback_ = callback;
    com::soc::SocketConfig cfg{kRx_sock_name, 0, 0};
    sock_.Init(cfg);
    sock_.SetRXCallback([this](const std::string& ip, const std::uint16_t& port,
                       std::vector<std::uint8_t> data) {
        RxCallback(ip, port, data);
    });
    sock_.StartRXThread();
    thread = std::jthread([this](const std::stop_token& t) {
        Loop(t);
    });
}

}  // namespace hb
}  // namespace core
}  // namespace srp
