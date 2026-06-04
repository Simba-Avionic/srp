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

#ifndef MW_TIMESTAMP_MW_NTP_DISCOVERY_DISCOVERY_MANAGER_HPP_
#define MW_TIMESTAMP_MW_NTP_DISCOVERY_DISCOVERY_MANAGER_HPP_

#include <string>
#include <unordered_map>
#include <chrono>  // NOLINT
#include <mutex>  // NOLINT
#include <thread>  // NOLINT
#include <cstdint>
#include <optional>

namespace srp {
namespace tinyNTP {

/**
 * @brief Pojedynczy węzeł sieci
 * 
 * ip - adres ip
 * ntp_class - klasa urządzenia (0-7), im niższa wartość, tym wyższy priorytet.
 * holdover - 1 = brak aktualnej referencji (czas niepewny); 0 = zsynchronizowany.
 * last_seen - czas otrzymania ostatniej wiadomości od danego urządzenia
 */
struct NodeInfo {
    std::string ip;
    uint8_t ntp_class;
    bool holdover;
    std::chrono::steady_clock::time_point last_seen;
};

class DiscoveryManager {
 private:
    NodeInfo local_node_;
    std::unordered_map<std::string, NodeInfo> neighbors_;  // ip -> NodeInfo

    void RemoveExpiredNodes();
    void cleanup_thread_loop(std::stop_token token);

    std::jthread cleanup_thread_;
    mutable std::mutex map_mutex_;
 public:
    DiscoveryManager() = default;
    ~DiscoveryManager() = default;

    void Init(const std::string& ip, const uint8_t ntp_class, const bool holdover);

    void UpdateNode(const std::string& ip, const uint8_t ntp_class, const bool holdover);
    std::optional<NodeInfo> GetBestMaster();
};

}  // namespace tinyNTP
}  // namespace srp

#endif  // MW_TIMESTAMP_MW_NTP_DISCOVERY_DISCOVERY_MANAGER_HPP_
