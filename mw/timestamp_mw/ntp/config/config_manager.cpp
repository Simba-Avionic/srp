/**
 * @file config_manager.cpp
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
    static constexpr auto kDefault_device_class = 7;
}  // namespace

std::optional<NtpConfig> ConfigManager::LoadConfig(const std::string& filepath) {
    NtpConfig config;

    auto parser_opt = srp::core::json::JsonParser::Parser(filepath);
    if (!parser_opt.has_value()) {
        ara::log::LogError() << "Cannot open or parse config file: " << filepath;
        return std::nullopt;
    }
    const auto& parser = parser_opt.value();

    auto ip = parser.GetString("ip");
    if (!ip) {
        ara::log::LogError() << "Cannot parse ip string in ntp configuration.";
        return std::nullopt;
    }
    config.ip = ip.value();

    auto ntp_class = parser.GetNumber<uint8_t>("ntp_class");
    if (!ntp_class) {
        ara::log::LogError() << "Cannot parse ntp_class in ntp configuration.";
        return std::nullopt;
    }
    config.ntp_class = ntp_class.value();
    if (config.ntp_class > 7) {
        ara::log::LogWarn() << "Config ntp_class > 7. Using fallback class 7.";
        config.ntp_class = kDefault_device_class;
    }

    auto t_hb_ms = parser.GetNumber<uint32_t>("T_hb_ms");
    if (!t_hb_ms) {
        ara::log::LogError() << "Cannot parse t_hb_ms in ntp configuration.";
        return std::nullopt;
    }
    config.t_hb_ms = t_hb_ms.value();

    return config;
}

}  // namespace tinyNTP
}  // namespace srp
