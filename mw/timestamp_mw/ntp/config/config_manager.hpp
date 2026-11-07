/**
 * @file config_manager.hpp
 * @brief Loads device configuration
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @version 0.1
 * @date 2026-05-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef MW_TIMESTAMP_MW_NTP_CONFIG_CONFIG_MANAGER_HPP_
#define MW_TIMESTAMP_MW_NTP_CONFIG_CONFIG_MANAGER_HPP_

#include <string>
#include <cstdint>
#include <optional>

namespace srp {
namespace tinyNTP {

constexpr auto kConfig_file_path = "/srp/opt/cpu_srp/ntp_config.json";

struct NtpConfig {
    std::string ip;
    uint8_t ntp_class;
    uint32_t t_hb_ms;
};

class ConfigManager {
 public:
    /**
     * @brief Wczytuje konfiguracje z pliku JSON
     * 
     * @param filepath Ścieżka do pliku konfiguracyjnego
     * @return NtpConfig Struktura z wczytanymi parametrami
     */
    static std::optional<NtpConfig> LoadConfig(const std::string& filepath = kConfig_file_path);
};

}  // namespace tinyNTP
}  // namespace srp

#endif  // MW_TIMESTAMP_MW_NTP_CONFIG_CONFIG_MANAGER_HPP_
