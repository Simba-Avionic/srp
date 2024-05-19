/**
 * @file controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <vector>
#include <string>
#include <thread>  // NOLINT
#include <future>  // NOLINT
#include <chrono> // NOLINT

#include "mw/i2c_service/controller/deltaPress/controller.hpp"
#include "core/json/json_parser.h"
#include "core/common/error_code.h"
#include "core/logger/Logger.h"
namespace simba {
namespace i2c {

DeltaPressureController::DeltaPressureController() {}

void DeltaPressureController::Init(const uint16_t &service_id,
                const std::unordered_map<std::string, std::string>& parms) {
  this->app_name = parms.at("app_name");
  this->adc_.Init(service_id);
  std::string file_path = "/opt/"+this->app_name+"/etc/config.json";
  std::ifstream file(file_path);
  if (!file.is_open()) {
      AppLogger::Warning("Cant find file on path "+file_path);
      return;
  }
  nlohmann::json data = nlohmann::json::parse(file);
  this->db_ = this->ReadConfig(data);
}
std::unordered_map<uint8_t, DeltaPressureSensorConfig> DeltaPressureController::ReadConfig(nlohmann::json data) {
    std::unordered_map<uint8_t, DeltaPressureSensorConfig> db;
    if (!data.contains("sensors")) {
        return {};
    }
    for (const auto &sensor : data["sensors"]) {
        if (!(sensor.contains("actuator_id") && sensor.contains("channel")
                && sensor.contains("a") && sensor.contains("b"))) {
            AppLogger::Warning("Invalid  pressure sensor config");
            continue;
        }
        DeltaPressureSensorConfig config;
        config.channel = sensor.at("channel");
        config.a = sensor.at("a");
        config.b = sensor.at("b");

        db.insert({sensor.at("actuator_id"), config});
    }
    return db;
}

std::optional<float> DeltaPressureController::GetDeltaPressure(const uint8_t sensor_id) {
    auto sensor = this->db_.find(sensor_id);
    if (sensor == db_.end()) {
        AppLogger::Warning("Invalid pressure sensor request");
        return {};
    }
    auto res = this->adc_.GetAdcVoltage(sensor->second.channel);
    if (!res.has_value()) {
        return {};
    }
    return sensor->second.a * res.value() + sensor->second.b;
}

}  // namespace i2c
}  // namespace simba

