/**
 * @file servo_config_reader.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_CONFIG_READER_HPP_
#define APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_CONFIG_READER_HPP_

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <utility>
#include <mutex>  // NOLINT

#include "core/json/json_parser.h"
#include "ara/log/log.h"

namespace srp {
namespace service {

struct ServoRuntimeConfig {
  using timepoint = std::chrono::high_resolution_clock::time_point;
  uint16_t on_pos;
  uint16_t off_pos;
  uint8_t channel;
  uint8_t mosfet_id;
  uint8_t ina219_i2c_address;
  uint16_t auto_closing;
  timepoint open_time_end;
  uint8_t position;


  ServoRuntimeConfig(const uint16_t on_pos_, const uint16_t off_pos_, const uint8_t channel_):
                        on_pos(on_pos_), off_pos(off_pos_), channel(channel_){}
};

struct servo_cfg {
    ServoRuntimeConfig cfg;
    uint16_t actuator_id;
};

class ConfigReader {
 private:
    std::unordered_map<uint8_t, ServoRuntimeConfig> db;
    mutable std::mutex db_mtx_;

    std::optional<servo_cfg> ReadServoConfig(core::json::JsonParser entry) const {
        auto actuator_id = entry.GetNumber<uint8_t>("actuator_id");
        auto channel = entry.GetNumber<uint8_t>("channel");
        auto on_pos = entry.GetNumber<uint16_t>("on_pos");
        auto off_pos = entry.GetNumber<uint16_t>("off_pos");
        if (!(actuator_id.has_value() && channel.has_value() && on_pos.has_value() &&
            off_pos.has_value())) {
            ara::log::LogWarn() << "ServoController.LoadConfig: incomplete servo description";
            return std::nullopt;
        }
        ServoRuntimeConfig cfg{on_pos.value(), off_pos.value(), channel.value()};

        cfg.mosfet_id = entry.GetNumber<uint8_t>("mosfet_id").value_or(0);
        cfg.ina219_i2c_address = entry.GetNumber<uint8_t>("ina219_address").value_or(0);
        cfg.auto_closing = entry.GetNumber<uint16_t>("auto_closing_time").value_or(0);

        ara::log::LogDebug() << "ServoController.LoadConfig: parsed actuator "
                             << std::to_string(static_cast<int>(actuator_id.value()))
                             << ", channel " << std::to_string(static_cast<int>(cfg.channel));
        return servo_cfg{cfg, actuator_id.value()};
    }
 public:
    std::optional<ServoRuntimeConfig> GetServoConfig(const uint8_t& actuator_id) {
        std::lock_guard<std::mutex> lock(db_mtx_);
        auto it = db.find(actuator_id);
        if (it == db.end()) {
            ara::log::LogWarn() << "ConfigReader.GetServoConfig: actuator not found "
                                << std::to_string(static_cast<int>(actuator_id));
            return std::nullopt;
        }
        ara::log::LogDebug() << "ConfigReader.GetServoConfig: returned actuator "
                             << std::to_string(static_cast<int>(actuator_id));
        return it->second;
    }

    std::vector<uint8_t> GetServosID() const {
        std::vector<uint8_t> res;
        std::lock_guard<std::mutex> lock(db_mtx_);
        for (const auto& servo: db) {
            res.push_back(servo.first);
        }
        return res;
    }

    bool ChangeServoConfigPosition(const uint8_t actuator_id, uint16_t new_open_val,
                                           uint16_t new_close_val) {
        std::lock_guard<std::mutex> lock(db_mtx_);
        auto it = db.find(actuator_id);
        if (it == db.end()) {
            ara::log::LogWarn() << "ConfigReader.ChangeServoConfigPosition: actuator not found "
                                << std::to_string(static_cast<int>(actuator_id));
            return false;
        }
        it->second.on_pos = new_open_val;
        it->second.off_pos = new_close_val;
        ara::log::LogInfo() << "ConfigReader.ChangeServoConfigPosition: actuator "
                            << std::to_string(static_cast<int>(actuator_id))
                            << " open=" << std::to_string(new_open_val)
                            << " close=" << std::to_string(new_close_val);
        return true;
    }

    bool SetServoPosition(const uint8_t& actuator_id, const uint8_t& state) {
        std::lock_guard<std::mutex> lock(db_mtx_);
        auto it = db.find(actuator_id);
        if (it == db.end()) {
            ara::log::LogWarn() << "ConfigReader.SetServoPosition: actuator not found "
                                << std::to_string(static_cast<int>(actuator_id));
            return false;
        }
        it->second.position = state;
        ara::log::LogDebug() << "ConfigReader.SetServoPosition: actuator "
                        << std::to_string(static_cast<int>(actuator_id))
                        << " state=" << std::to_string(static_cast<int>(state));
        if (it->second.auto_closing == 0) {
            return true;
        }

        if (state == 1) {
            auto new_closing_time = std::chrono::high_resolution_clock::now();
            new_closing_time += std::chrono::milliseconds(it->second.auto_closing);
            it->second.open_time_end = std::max(it->second.open_time_end, new_closing_time);
        }
        return true;
    }

    void LoadServoConfig(const std::string& path) {
        ara::log::LogInfo() << "ServoController.LoadConfig: start loading from " << path;
        auto parser = core::json::JsonParser::Parser(path);
        if (!parser.has_value()) {
            ara::log::LogError() << "ServoController.LoadConfig: missing config file at " << path;
            return;
        }
        auto array = parser.value().GetArray<nlohmann::json>("servos");
        if (!array.has_value()) {
            ara::log::LogError() << "ServoController.LoadConfig: missing 'servos' array in config";
            return;
        }
        for (auto& entry : array.value()) {
            auto servo_parser_opt = core::json::JsonParser::Parser(entry);
            if (!servo_parser_opt.has_value()) {
                ara::log::LogWarn() << "ServoController.LoadConfig: invalid servo entry";
                continue;
            }
            auto cfg = ReadServoConfig(servo_parser_opt.value());
            if (cfg.has_value()) {
                {
                    std::unique_lock<std::mutex> lock(db_mtx_);
                    db.emplace(cfg.value().actuator_id, cfg.value().cfg);
                }
                ara::log::LogInfo() << "ServoController.LoadConfig: registered actuator " <<
                                    std::to_string(static_cast<int>(cfg.value().actuator_id));
            }
        }
        ara::log::LogInfo() << "ServoController.LoadConfig: loaded "
                            << std::to_string(static_cast<int>(db.size()))
                            << " actuators";
    }
};

}
}
#endif  // APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_CONFIG_READER_HPP_
