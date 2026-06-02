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

#include "mw/timestamp_mw/ntp/config/config_manager.hpp"
#include "core/json/json_parser.h"
#include "ara/log/log.h"

namespace srp {
namespace tinyNTP {
namespace {
    static constexpr auto kDefault_ip = "127.0.0.1";
    static constexpr auto kDefault_device_class = 7;
    static constexpr auto kDefault_announce_interval = 1000;
}  // namespace

NtpConfig ConfigManager::LoadConfig(const std::string& filepath) {
    NtpConfig config;
    // Podstawowe dane do fallbacku
    config.ip = "127.0.0.1";
    config.ntp_class = 7;
    config.t_hb_ms = 1000;

    auto parser_opt = srp::core::json::JsonParser::Parser(filepath);
    if (!parser_opt.has_value()) {
        ara::log::LogError() << "Cannot open or parse config file: " << filepath
                  << ". Using fallback values.";
        return config;
    }
    const auto& parser = parser_opt.value();

    auto ip = parser.GetString("ip");
    config.ip = ip.value_or(kDefault_ip);

    auto ntp_class = parser.GetNumber<uint8_t>("ntp_class");
    config.ntp_class = ntp_class.value_or(kDefault_device_class);
    if (config.ntp_class > 7) {
        ara::log::LogWarn() << "Config ntp_class > 7. Using fallback class 7.";
        config.ntp_class = kDefault_device_class;
    }

    auto t_hb_ms = parser.GetNumber<uint32_t>("T_hb_ms");
    config.t_hb_ms = t_hb_ms.value_or(kDefault_announce_interval);

    return config;
}

}  // namespace tinyNTP
}  // namespace srp
