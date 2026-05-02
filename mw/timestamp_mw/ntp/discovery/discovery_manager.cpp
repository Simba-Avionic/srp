/**
 * @file discovery_manager.hpp
 * @brief Manages the selection of the best time provider
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @version 0.1
 * @date 2026-05-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "mw/timestamp_mw/ntp/discovery/discovery_manager.hpp"

namespace srp {
namespace tinyNTP {

/**
 * @brief Usuwa nieaktywne węzły z mapy urządzeń sieciowych
 * 
 * @warning Funkcja nie jest thread-safe.
 * Przed wywołaniem funkcji należy zablokować `map_mutex_`.
 * 
 * @param timeout_seconds Czas wygaśnięcia sąsiada
 */
void DiscoveryManager::RemoveExpiredNodes(int timeout_seconds) {
    const auto now = std::chrono::steady_clock::now();

    for (auto it = neighbors_.begin(); it != neighbors_.end(); ) {
        const auto delta = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.last_seen).count();

        if (delta > timeout_seconds) {
            it = neighbors_.erase(it);
        } else {
            ++it;
        }
    }
}

void DiscoveryManager::UpdateNode(const std::string& ip, uint8_t ntp_class, bool holdover) {
    std::lock_guard<std::mutex> lock(map_mutex_);

    NodeInfo& node = neighbors_[ip];
    
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
std::optional<NodeInfo> DiscoveryManager::GetBestMaster(const NodeInfo& local_node) {
    std::lock_guard<std::mutex> lock(map_mutex_);

    RemoveExpiredNodes();

    NodeInfo best_neighbor = local_node;

    for (const auto& [ip, node] : neighbors_) {
        if (node.ntp_class != best_neighbor.ntp_class) {
            if (node.ntp_class < best_neighbor.ntp_class) {
                best_neighbor = node;
            }
        } 
        else if (node.holdover != best_neighbor.holdover) {
            if (!node.holdover) {
                best_neighbor = node;
            }
        }
        else if (node.ip < best_neighbor.ip) {
            best_neighbor = node;
        } 

    }

    if (best_neighbor.ip == local_node.ip) return std::nullopt;
    return best_neighbor;
}

} // namespace tinyNTP
} // namespace srp