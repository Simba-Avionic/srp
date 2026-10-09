/**
 * @file fuel_config_reader.hpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef APPS_EC_FUEL_SERVICE_FUEL_CONFIG_READER_HPP_
#define APPS_EC_FUEL_SERVICE_FUEL_CONFIG_READER_HPP_

#include <vector>
#include <string>
#include <optional>
#include "core/json/json_parser.h"
#include "ara/log/log.h"
#include "apps/ec/fuel_service/controller/refuel_controller.hpp"

namespace srp {
namespace apps {

struct TankConfig : public Config {
    uint8_t tank_id;
    RefuelingType_t refueling_type;
    uint8_t gs_main_valve_id;
    std::optional<uint8_t> rocket_vent_valve_id;
    uint8_t rocket_dump_valve_id;
};

class FuelConfigReader {
 public:
    static std::optional<std::vector<TankConfig>> LoadConfig(const std::string& path) {
        std::vector<TankConfig> configs;
        auto parser_opt = core::json::JsonParser::Parser(path);
        if (!parser_opt.has_value()) {
            ara::log::LogFatal() << "FuelConfigReader: Failed to open config file at " << path;
            return std::nullopt;
        }

        auto tanks_array = parser_opt.value().GetArray<nlohmann::json>("tanks");
        if (!tanks_array.has_value()) {
            ara::log::LogFatal() << "FuelConfigReader: 'tanks' array not found in config!";
            return std::nullopt;
        }

        size_t index = 0;
        for (const auto& entry : tanks_array.value()) {
            auto tank_parser_opt = core::json::JsonParser::Parser(entry);
            if (!tank_parser_opt.has_value()) {
                ara::log::LogFatal() << "FuelConfigReader: Failed to parse tank entry at index " << index;
                return std::nullopt;
            }
            auto parser = tank_parser_opt.value();

            TankConfig cfg;

            auto t_id = parser.GetNumber<uint8_t>("tank_id");
            if (!t_id) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'tank_id' at index " << index;
                return std::nullopt;
            }
            cfg.tank_id = t_id.value();
            const int id_log = static_cast<int>(cfg.tank_id);

            auto t_name = parser.GetString("tank_name");
            if (!t_name) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'tank_name' for tank_id: " << id_log;
                return std::nullopt;
            }
            cfg.tank_name = t_name.value();

            auto r_type = parser.GetString("refueling_type");
            if (!r_type) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'refueling_type' for tank_id: " << id_log;
                return std::nullopt;
            }
            if (r_type.value() != "PRESSURE" && r_type.value() != "MASS") {
                ara::log::LogFatal() << "FuelConfigReader: Invalid 'refueling_type' for tank_id: " << id_log;
                return std::nullopt;
            }
            cfg.refueling_type = (r_type.value() == "PRESSURE") ? RefuelingType_t::PRESSURE : RefuelingType_t::MASS;

            auto min_gs = parser.GetNumber<uint16_t>("min_gs_pressure");
            if (!min_gs) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'min_gs_pressure' for tank_id: "
                    << id_log;
                return std::nullopt;
            }
            cfg.min_gs_pressure = min_gs.value();

            auto max_gs = parser.GetNumber<uint16_t>("max_gs_pressure");
            if (!max_gs) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'max_gs_pressure' for tank_id: "
                    << id_log;
                return std::nullopt;
            }
            cfg.max_gs_pressure = max_gs.value();

            auto t_press = parser.GetNumber<uint16_t>("test_pressure");
            if (!t_press) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'test_pressure' for tank_id: " << id_log;
                return std::nullopt;
            }
            cfg.test_pressure = t_press.value();

            auto max_drop = parser.GetNumber<uint8_t>("test_max_drop_percent");
            if (!max_drop) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'test_max_drop_percent' for tank_id: "
                    << id_log;
                return std::nullopt;
            }
            cfg.test_max_drop_percent = max_drop.value();

            auto max_safe = parser.GetNumber<uint16_t>("max_safe_pressure");
            if (!max_safe) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'max_safe_pressure' for tank_id: "
                    << id_log;
                return std::nullopt;
            }
            cfg.max_safe_pressure = max_safe.value();

            auto max_time = parser.GetNumber<uint16_t>("max_tanking_time_s");
            if (!max_time) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'max_tanking_time_s' for tank_id: "
                    << id_log;
                return std::nullopt;
            }
            cfg.max_tanking_time_s = max_time.value();

            auto valves_opt = parser.GetObject("valves");
            if (!valves_opt) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'valves' object for tank_id: " << id_log;
                return std::nullopt;
            }

            auto valves = valves_opt.value();

            auto gs_main = valves.GetNumber<uint8_t>("gs_main");
            if (!gs_main) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'gs_main' in valves for tank_id: "
                    << id_log;
                return std::nullopt;
            }
            cfg.gs_main_valve_id = gs_main.value();

            auto r_dump = valves.GetNumber<uint8_t>("rocket_dump");
            if (!r_dump) {
                ara::log::LogFatal() << "FuelConfigReader: Missing or invalid 'rocket_dump' in valves for tank_id: "
                    << id_log;
                return std::nullopt;
            }
            cfg.rocket_dump_valve_id = r_dump.value();

            cfg.rocket_vent_valve_id = valves.GetNumber<uint8_t>("rocket_vent");

            configs.push_back(cfg);
            index++;
        }

        return configs;
    }
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_FUEL_SERVICE_FUEL_CONFIG_READER_HPP_
