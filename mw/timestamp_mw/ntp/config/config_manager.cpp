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
#include <iostream>

namespace srp {
namespace tinyNTP {

NtpConfig ConfigManager::LoadConfig(const std::string& filepath) {
    NtpConfig config;
    // Podstawowe dane do fallbacku
    config.ip = "127.0.0.1";
    config.ntp_class = 7;
    config.t_hb_ms = 1000;

    auto parser_opt = srp::core::json::JsonParser::Parser(filepath);
    
    if (!parser_opt.has_value()) {
        ara::log::LogError() << "Cannot open or parse config file: " << filepath 
                  << ". Using fallback values.\n";
        return config;
    }

    const auto& parser = parser_opt.value();

    auto ip_opt = parser.GetString("ip");
    if (ip_opt.has_value()) {
        config.ip = ip_opt.value();
    } else {
        ara::log::LogWarn() << "Missing or invalid 'ip'. Using fallback ip.\n";
    }

    auto raw_json = parser.GetObject();

    if (raw_json.contains("ntp_class") && raw_json["ntp_class"].is_number_integer()) {
        config.ntp_class = raw_json["ntp_class"];
        
        if (config.ntp_class > 7) {
            ara::log::LogWarn() << "Config ntp_class > 7. Using fallback class 7.\n";
            config.ntp_class = 7;
        }
    } else {
        ara::log::LogWarn() << "Missing or invalid 'ntp_class'. Using fallback class 7.\n";
    }

    if (raw_json.contains("T_hb_ms") && raw_json["T_hb_ms"].is_number_integer()) {
        config.t_hb_ms = raw_json["T_hb_ms"];
    } else {
        ara::log::LogWarn() << "Missing or invalid 'T_hb_ms'. Using fallback T_hb_ms.\n";
    }

    return config;
}

}  // namespace tinyNTP
}  // namespace srp