/**
 * @file discovery_manager.cpp
 * @brief Manages the selection of the best time provider
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @version 0.1
 * @date 2026-05-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "mw/timestamp_mw/ntp/discovery/discovery_manager.hpp"
#include <cstdio>
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace srp {
namespace tinyNTP {
namespace {
    static constexpr auto kTimeout_seconds = 15;
}  // namespace

std::optional<uint32_t> DiscoveryManager::IpToUint32(const std::string& ip) {
    uint32_t a, b, c, d;
    if (sscanf(ip.c_str(), "%u.%u.%u.%u", &a, &b, &c, &d) == 4) {
        return (a << 24) | (b << 16) | (c << 8) | d;
    }
    return std::nullopt;
}

void DiscoveryManager::Init(const std::string& ip, const uint8_t ntp_class, const bool holdover) {
    const auto ip_numeric_opt = IpToUint32(ip);
    if (!ip_numeric_opt.has_value()) {
        ara::log::LogError() << "Discovery Manager init failed. Provided invalid ip string for local node.";
        return;
    }

    local_node_.ip = ip;
    local_node_.ip_numeric = ip_numeric_opt.value();
    local_node_.ntp_class = ntp_class;
    local_node_.holdover = holdover;
    local_node_.last_seen = std::chrono::steady_clock::now();

    cleanup_thread_ = std::jthread([this](std::stop_token token) {
        cleanup_thread_loop(token);
    });
}

void DiscoveryManager::SetLocalNodeHoldover(const bool newHoldover) {
    std::lock_guard<std::mutex> lock(map_mutex_);

    local_node_.holdover = newHoldover;
}


/**
 * @brief Usuwa nieaktywne węzły z mapy urządzeń sieciowych
 */
void DiscoveryManager::RemoveExpiredNodes() {
    std::lock_guard<std::mutex> lock(map_mutex_);

    const auto now = std::chrono::steady_clock::now();

    for (auto it = neighbors_.begin(); it != neighbors_.end(); ) {
        const auto delta = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.last_seen).count();

        if (delta > kTimeout_seconds) {
            ara::log::LogDebug() << "Removing expired node with IP: " << it->first;
            it = neighbors_.erase(it);
        } else {
            ++it;
        }
    }
}

void DiscoveryManager::cleanup_thread_loop(std::stop_token token) {
    ara::log::LogInfo() << "Start cleanup thread";
    const auto check_interval = std::chrono::seconds(1);

    while (!token.stop_requested()) {
        RemoveExpiredNodes();
        core::condition::wait_for(check_interval, token);
    }
}

void DiscoveryManager::UpdateNode(const std::string& ip, const uint8_t ntp_class, const bool holdover) {
    const auto ip_numeric_opt = IpToUint32(ip);
    if (!ip_numeric_opt.has_value()) {
        ara::log::LogError() << "Provided invalid ip string for node. Node is not added to discovery.";
        return;
    }

    std::lock_guard<std::mutex> lock(map_mutex_);

    auto result = neighbors_.insert({ip, NodeInfo{}});
    NodeInfo& node = result.first->second;

    node.ip = ip;
    node.ip_numeric = ip_numeric_opt.value();
    node.ntp_class = ntp_class;
    node.holdover = holdover;
    node.last_seen = std::chrono::steady_clock::now();
}

/**
 * @brief Zwraca najlepszy węzeł (Master) wykryty w sieci 
 * lub std::nullopt w przypadku gdy węzeł lokalny jest najlepszy.
 * @return std::optional<NodeInfo> Najlepszy zewnętrzny Master. Zwraca std::nullopt, 
 * jeśli sieć jest pusta lub to węzeł lokalny ma najlepsze parametry w sieci.
 */
std::optional<NodeInfo> DiscoveryManager::GetBestMaster() {
    std::lock_guard<std::mutex> lock(map_mutex_);

    NodeInfo best_neighbor = local_node_;

    for (const auto& [ip, node] : neighbors_) {
        if (node.ntp_class != best_neighbor.ntp_class) {
            if (node.ntp_class < best_neighbor.ntp_class) {
                best_neighbor = node;
            }
        } else if (node.holdover != best_neighbor.holdover) {
            if (!node.holdover) {
                best_neighbor = node;
            }
        } else if (node.ip_numeric < best_neighbor.ip_numeric) {
            best_neighbor = node;
        }
    }

    if (best_neighbor.ip == local_node_.ip) return std::nullopt;
    return best_neighbor;
}

}  // namespace tinyNTP
}  // namespace srp
