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
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace srp {
namespace tinyNTP {
namespace {
    constexpr auto kTimeout_seconds = 15;
}

void DiscoveryManager::Init(const std::string& ip, const uint8_t ntp_class, const bool holdover) {
    local_node_ = NodeInfo{ip, ntp_class, holdover};

    cleanup_thread_ = std::jthread([this](std::stop_token token) {
        cleanup_thread_loop(token);
    });
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
    std::lock_guard<std::mutex> lock(map_mutex_);

    auto result = neighbors_.insert({ip, NodeInfo{}});
    NodeInfo& node = result.first->second;

    node.ip = ip;
    node.ntp_class = ntp_class;
    node.holdover = holdover;
    node.last_seen = std::chrono::steady_clock::now();
}

/**
 * @brief Zwraca najlepszy węzeł w sieci
 * 
 * @param local_node 
 * @return std::optional<NodeInfo> - W przypadku gdy lokalny node jest najlepszym w sieci zwrócony optional jest pusty
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
        } else if (node.ip < best_neighbor.ip) {
            best_neighbor = node;
        }
    }

    if (best_neighbor.ip == local_node_.ip) return std::nullopt;
    return best_neighbor;
}

}  // namespace tinyNTP
}  // namespace srp
