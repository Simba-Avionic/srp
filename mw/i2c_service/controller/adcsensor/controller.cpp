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

float ADCSensorController::CalculateA(float R, float RES_MAX, float RES_MIN, float A_MAX, float A_MIN) const {
    return (RES_MAX - RES_MIN) * 1000.0f / ((A_MAX - A_MIN) * R);
}

float ADCSensorController::CalculateB(float R, float A, float A_MIN, float RES_MIN) const {
    return RES_MIN - ((A_MIN/1000.0f)*R*A);
}

ADCSensorController::ADCSensorController() {}

core::ErrorCode ADCSensorController::Init(const std::unordered_map<std::string, std::string>& parms,
    std::unique_ptr<IADS7828> adc_) {
  this->app_name = parms.at("app_name");
  std::string file_path = "/opt/"+this->app_name+"/etc/config.json";
  auto obj_r = core::json::JsonParser::Parser(file_path);
  if (!obj_r.has_value()) {
    AppLogger::Warning("Cant find file on path "+file_path);
    return core::ErrorCode::kInitializeError;
  }
  if (this->setPtr(std::move(adc_)) != core::ErrorCode::kOk) {
    AppLogger::Warning("ADS7828 initialize error");
    return core::ErrorCode::kInitializeError;
  }
  this->db_ = this->ReadConfig(obj_r.value());
  return core::ErrorCode::kOk;
}

core::ErrorCode ADCSensorController::setPtr(std::unique_ptr<IADS7828> adc_) {
  if (!adc_) {
    return core::ErrorCode::kInitializeError;
  }
  this->adc_ = std::move(adc_);
  return core::ErrorCode::kOk;
}

void ADCSensorController::setConfig(const std::unordered_map<uint8_t, SensorConfig>& db_) {
    this->db_ = db_;
}

std::unordered_map<uint8_t, SensorConfig> ADCSensorController::ReadConfig(core::json::JsonParser parser) const {
    std::unordered_map<uint8_t, SensorConfig> db;
    auto sensors_opt = parser.GetArray("sensors");
    if (!sensors_opt.has_value()) {
        AppLogger::Error("Invalid config in adcSensor");
        exit(1);
    }
    for (const auto &sensor__ : sensors_opt.value()) {
        auto parser_opt = core::json::JsonParser::Parser(sensor__);
        if (!parser_opt.has_value()) {
            continue;
        }
        auto parser = parser_opt.value();
        auto actuator_id = parser.GetNumber<uint8_t>("actuator_id");
        auto channel = parser.GetNumber<uint8_t>("channel");
        if (!(actuator_id.has_value() && channel.has_value())) {
            AppLogger::Warning("Missing actuator_id or channel");
            continue;
        }
        auto a = parser.GetNumber<float>("a");
        auto b = parser.GetNumber<float>("b");
        SensorConfig config;
        config.channel = channel.value();
        if (a.has_value() && b.has_value()) {
            config.a = a.value();
            config.b = b.value();
            db.insert({actuator_id.value(), config});
        } else {
            auto r = parser.GetNumber<float>("R");
            auto resMin = parser.GetNumber<float>("RES_MIN");
            auto resMax = parser.GetNumber<float>("RES_MAX");
            auto AMin = parser.GetNumber<float>("A_MIN");
            auto AMax = parser.GetNumber<float>("A_MAX");
            if (!r.has_value() || !resMin.has_value() || !resMax.has_value()
            || !AMax.has_value() || !AMin.has_value()) {
                AppLogger::Warning("Invalid config for actuatorID:"
                                            + std::to_string(actuator_id.value()));
                continue;
            }
            config.a = CalculateA(r.value(), resMax.value(), resMin.value(), AMax.value(), AMin.value());
            config.b = CalculateB(r.value(), config.a, AMin.value(), resMin.value());
            db.insert({actuator_id.value(), config});
        }
    }
    return db;
}

std::optional<float> ADCSensorController::GetValue(const uint8_t sensor_id) const {
    auto sensor = this->db_.find(sensor_id);
    if (sensor == db_.end()) {
        AppLogger::Warning("Invalid pressure sensor request");
        return {};
    }
    auto res = this->adc_->GetAdcVoltage(sensor->second.channel);
    if (!res.has_value()) {
        return {};
    }
    return sensor->second.a * res.value() + sensor->second.b;
}

}  // namespace i2c
}  // namespace simba
