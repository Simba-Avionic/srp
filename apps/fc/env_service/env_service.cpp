/**
 * @file env_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <sstream>
#include <iomanip>
#include "apps/fc/env_service/env_service.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"
#include "srp/env/EnvAppFcSkeleton.h"
#include "core/json/json_parser.h"
// #include "mw/i2c_service/controller/ads7828/controller.hpp"
#include "mw/i2c_service/controller/bme280/controller.hpp"

namespace srp {
namespace envService {

namespace {
    constexpr uint8_t PRESS_SENSOR_ID = 10;
    constexpr uint8_t PRESS_SENSOR_SAMPLING = 15;
    constexpr auto kDelay = 500;
}

EnvService::EnvService(): service_ipc(ara::core::InstanceSpecifier{"srp/fc/env/EnvApp/envService_ipc"}),
                  service_udp(ara::core::InstanceSpecifier{"srp/fc/env/EnvApp/envService_udp"}) {
}


core::ErrorCode EnvService::Init(std::unique_ptr<mw::temp::TempController> temp) {
    if (this->temp_ || !temp) {
      return core::ErrorCode::kInitializeError;
    }
    this->temp_ = std::move(temp);
    return core::ErrorCode::kOk;
}



int EnvService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
    ara::log::LogInfo() << "EnvService has been started";
    // Check if app_path exists in parameters
    auto app_path_it = parms.find("app_path");
    if (app_path_it == parms.end()) {
        ara::log::LogError() << "app_path parameter not found in parms";
        return core::ErrorCode::kInitializeError;
    }

    // Initialize pressure sensor
    bme = std::make_unique<i2c::BME280>();
    auto i2c = std::make_unique<i2c::I2CController>();
    if(i2c->Init(std::make_unique<com::soc::StreamIpcSocket>()) != core::ErrorCode::kOk){
        ara::log::LogError() << "EnvApp: Failed to initialize i2c pointer";
    } else {
        ara::log::LogDebug() << "EnvApp: Initialized i2c pointer";
    }
    ara::log::LogDebug() << "EnvApp: init Complete";
    if(bme->Init(std::move(i2c)) != core::ErrorCode::kOk){
        ara::log::LogWarn() << "EnvApp: Failed to initialize BME280 sensor";
    }
    ara::log::LogInfo() << "EnvApp: Pressure initialization Complete";

    auto temp_init_res = this->Init(std::make_unique<mw::temp::TempController>());
    if (temp_init_res != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize TempController";
        return core::ErrorCode::kInitializeError;
    }
    core::ErrorCode res;
    uint8_t i = 0;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        res = this->temp_->Initialize(514, std::bind(&EnvService::TempRxCallback,
            this, std::placeholders::_1), std::make_unique<com::soc::StreamIpcSocket>());
        i++;
    } while (res != core::ErrorCode::kOk && i < 6);
    LoadTempConfig(parms);
    service_ipc.StartOffer();
    service_udp.StartOffer();
    return core::ErrorCode::kOk;
}

int EnvService::LoadTempConfig(const std::map<ara::core::StringView, ara::core::StringView>& parms) {
    ara::log::LogInfo() << "Starting function LoadTempConfig";
    const std::string path = parms.at("app_path") + "etc/config.json";
    auto parser_opt = core::json::JsonParser::Parser(path);
    ara::log::LogInfo() << path;
    if (!parser_opt.has_value()) {
        ara::log::LogError() <<("Failed to open temp_Service config file");
        exit(1);
    }
    ara::log::LogInfo() << "Opened file";
    auto temp_opt = parser_opt.value().GetArray<nlohmann::json>("sensors-temp");
    if (!temp_opt.has_value()) {
        ara::log::LogError() <<("Invalid temp_Service config format");
        exit(2);
    }
    for (const auto &data : temp_opt.value()) {
        auto parser_opt = core::json::JsonParser::Parser(data);
        if (!parser_opt.has_value()) {
            continue;
        }
        auto parser = parser_opt.value();
        auto physical_id = parser.GetString("physical_id");
        auto name = parser.GetString("name");
        if (!physical_id.has_value() || !name.has_value()) {
            continue;
        }

        ara::log::LogDebug() << "Sending subscribe request to temp_service";
        std::optional<uint8_t> sensor_id = this->temp_->Register(physical_id.value());
        if(!sensor_id.has_value()){
            ara::log::LogError() << "Sensor_id is empty";
            continue;
        }
        sensorIdsToPaths[sensor_id.value()] = std::make_pair(name.value(), physical_id.value());
    }
    return srp::core::ErrorCode::kOk;
}

int EnvService::Run(const std::stop_token& token) {
    while (!token.stop_requested()) {
        auto start = std::chrono::high_resolution_clock::now();
        float pressValue = 0.0f, tempValue = 0.0f, humValue = 0.0f;
        auto pressErr = this->bme->getPressure(pressValue);
        auto tempErr = this->bme->getTemperature(tempValue);
        auto humErr = this->bme->getHumidity(humValue);
        if (pressErr == core::ErrorCode::kOk && tempErr == core::ErrorCode::kOk && humErr == core::ErrorCode::kOk) {
            //TODO(gregority9@gmail.com) calculate altitude from pressure and broadcast altitude
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << pressValue;
            ara::log::LogInfo() << "Receive new Tank Pressure: " << ss.str() << " Bar";
            this->service_ipc.newBME280Event.Update({tempValue, humValue, pressValue});
            this->service_udp.newBME280Event.Update({tempValue, humValue, pressValue});
        } else {
            ara::log::LogWarn() << "Error getting data from BME sensor";
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        ara::log::LogDebug() << "loop taken:" << std::to_string(duration.count()) << "ms";
        core::condition::wait_for(std::chrono::milliseconds(kDelay - duration.count()), token);  // 1 Hz
    }
    service_ipc.StopOffer();
    service_udp.StopOffer();
    return core::ErrorCode::kOk;
}

void EnvService::TempRxCallback(const std::vector<srp::mw::temp::TempReadHdr>& data) {
    for (auto &hdr : data) {
        const int16_t value = static_cast<int16_t>(hdr.value * 10);
        ara::log::LogDebug() << "Receive temp id: " << hdr.actuator_id << ", name: " << sensorIdsToPaths[hdr.actuator_id].first << ", temp: " << static_cast<float>(value/10);
        if(sensorIdsToPaths[hdr.actuator_id].first == "sensor_temp_1"){
            this->service_ipc.newBoardTempEvent_1.Update(value);
            this->service_udp.newBoardTempEvent_1.Update(value);
        }
        else if(sensorIdsToPaths[hdr.actuator_id].first == "sensor_temp_2"){
            this->service_ipc.newBoardTempEvent_2.Update(value);
            this->service_udp.newBoardTempEvent_2.Update(value);
        }
        else if(sensorIdsToPaths[hdr.actuator_id].first == "sensor_temp_3"){
            this->service_ipc.newBoardTempEvent_3.Update(value);
            this->service_udp.newBoardTempEvent_3.Update(value);
        }
        else {
            ara::log::LogWarn() << "ID spoza zakresu:" << hdr.actuator_id;
        }
    }
}




}  // namespace envService
}  // namespace srp
