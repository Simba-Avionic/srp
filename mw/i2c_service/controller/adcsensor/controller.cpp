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

#include "mw/i2c_service/controller/adcsensor/controller.hpp"
#include "core/json/json_parser.h"
#include "core/common/error_code.h"
#include "core/logger/Logger.h"
namespace simba {
namespace i2c {

float ADCSensorController::calculateA(float R, float RES_MAX, float RES_MIN, float A_MAX, float A_MIN) const {
    return (RES_MAX - RES_MIN) * 1000.0f / ((A_MAX - A_MIN) * R);
}

float ADCSensorController::calculateB(float R, float A, float A_MIN) const {
    return -((A * A_MIN * R) / 1000.0f);
}

ADCSensorController::ADCSensorController() {}

void ADCSensorController::Init(const std::unordered_map<std::string, std::string>& parms) {
  this->app_name = parms.at("app_name");
  std::string file_path = "/opt/"+this->app_name+"/etc/config.json";
  std::ifstream file(file_path);
  if (!file.is_open()) {
      AppLogger::Warning("Cant find file on path "+file_path);
      return;
  }
  nlohmann::json data = nlohmann::json::parse(file);
  this->db_ = this->ReadConfig(data);
}
std::unordered_map<uint8_t, SensorConfig> ADCSensorController::ReadConfig(nlohmann::json data) {
    std::unordered_map<uint8_t, SensorConfig> db;
    if (!data.contains("sensors")) {
        return {};
    }
    for (const auto &sensor : data["sensors"]) {
        if (!(sensor.contains("actuator_id") && sensor.contains("channel"))) {
            AppLogger::Warning("Invalid  pressure sensor config");
            continue;
        }
        SensorConfig config;
        config.channel = sensor.at("channel");
        if ((sensor.contains("a") && sensor.contains("b"))) {
            config.a = sensor.at("a");
            config.b = sensor.at("b");
        } else if (sensor.contains("R") && sensor.contains("RES_MIN") &&
            sensor.contains("RES_MAX") && sensor.contains("A_MAX") && sensor.contains("A_MIN")) {
            config.a = calculateA(sensor.at("R"), sensor.at("RES_MAX"),
                        sensor.at("RES_MIN"), sensor.at("A_MAX"), sensor.at("A_MIN"));
            config.b = calculateB(sensor.at("R"), config.a, sensor.at("A_MIN"));
        } else {
            AppLogger::Warning("Invalid config for sensor with id:"+
                                std::to_string(static_cast<int>(sensor.at("actuator_id"))));
            continue;
        }
        db.insert({sensor.at("actuator_id"), config});
    }
    return db;
}

std::optional<float> ADCSensorController::GetValue(const uint8_t sensor_id) {
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

