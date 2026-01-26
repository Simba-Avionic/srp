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
#include "srp/env/EnvAppSkeleton.h"
#include "mw/i2c_service/controller/ads7828/controller.hpp"

namespace srp {
namespace envService {

namespace {
    constexpr uint8_t PRESS_SENSOR_ID = 10;
    constexpr uint8_t PRESS_SENSOR_SAMPLING = 15;
    constexpr auto kDelay = 500;
}



core::ErrorCode EnvService::Init(std::unique_ptr<mw::temp::TempController> temp) {
    if (this->temp_ || !temp) {
      return core::ErrorCode::kInitializeError;
    }
    this->temp_ = std::move(temp);
    return core::ErrorCode::kOk;
}

EnvService::EnvService(): press_{std::move(std::make_shared<i2c::ADCSensorController>())},
                service_ipc{press_, ara::core::InstanceSpecifier{"srp/env/EnvApp/envService_ipc"}, PRESS_SENSOR_ID},
                service_udp{press_, ara::core::InstanceSpecifier{"srp/env/EnvApp/envService_udp"}, PRESS_SENSOR_ID} {
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
    auto adc = std::make_unique<i2c::ADS7828>();
    auto i2c = std::make_unique<i2c::I2CController>();
    i2c->Init(std::make_unique<com::soc::StreamIpcSocket>());
    auto adc_init_res = adc->Init(std::move(i2c));
    if (adc_init_res != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize ADC";
        return core::ErrorCode::kInitializeError;
    }

    // Convert StringView to string for Init
    std::string app_path_str(app_path_it->second.data(), app_path_it->second.size());
    auto press_init_res = this->press_->Init(app_path_str, std::move(adc));
    if (press_init_res != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize pressure sensor controller";
        return core::ErrorCode::kInitializeError;
    }

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
        float press_sum = 0.0f;
        float num = 0.0f;
        for (uint8_t i = 0; i < PRESS_SENSOR_SAMPLING; i++) {
            auto pressValue = this->press_->GetValue(PRESS_SENSOR_ID);
            if (pressValue.has_value()) {
                press_sum += pressValue.value();
                num += 1.0f;
            } else {
                ara::log::LogWarn() << "dont receive new pressure";
            }
        }
        if (num > 0.0f) {
            float mean_press = press_sum / num;
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << mean_press;
            ara::log::LogInfo() << "Receive new Tank Pressure: " << ss.str() << " Bar";
            this->service_ipc.newPressEvent.Update(static_cast<uint16_t>(mean_press * 100));
            this->service_udp.newPressEvent.Update(static_cast<uint16_t>(mean_press * 100));
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
            this->service_ipc.newTempEvent_1.Update(value);
            this->service_udp.newTempEvent_1.Update(value);
        }
        else if(sensorIdsToPaths[hdr.actuator_id].first == "sensor_temp_2"){
            this->service_ipc.newTempEvent_2.Update(value);
            this->service_udp.newTempEvent_2.Update(value);
        }
        else if(sensorIdsToPaths[hdr.actuator_id].first == "sensor_temp_3"){
            this->service_ipc.newTempEvent_3.Update(value);
            this->service_udp.newTempEvent_3.Update(value);
        }
        else {
            ara::log::LogWarn() << "ID spoza zakresu:" << hdr.actuator_id;
        }
    }
}




}  // namespace envService
}  // namespace srp
