/**
 * @file env_service.cpp
 * @author Grzegorz Horbaczewski (gregority9@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
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
#include "core/json/json_parser.h"
#include "mw/i2c_service/controller/bme280/controller.hpp"

namespace srp {
namespace envServiceFc {

namespace {
    static constexpr auto kBME280_delay_ms =           100;
    static constexpr auto kService_instance_path_ipc = "srp/env/EnvAppFc/envServiceFc_ipc";
    static constexpr auto kService_instance_path_udp = "srp/env/EnvAppFc/envServiceFc_udp";
}  // namespace

EnvServiceFc::EnvServiceFc(): service_ipc(ara::core::InstanceSpecifier{kService_instance_path_ipc}),
                  service_udp(ara::core::InstanceSpecifier{kService_instance_path_udp}) {
}


core::ErrorCode EnvServiceFc::Init(std::unique_ptr<mw::temp::TempController> temp) {
    if (this->temp_ || !temp) {
      return core::ErrorCode::kInitializeError;
    }
    this->temp_ = std::move(temp);
    return core::ErrorCode::kOk;
}



int EnvServiceFc::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
    ara::log::LogInfo() << "EnvService has been started";
    // Check if app_path exists in parameters
    auto app_path_it = parms.find("app_path");
    if (app_path_it == parms.end()) {
        ara::log::LogError() << "app_path parameter not found in parms";
        return core::ErrorCode::kInitializeError;
    }

    // Initialize pressure sensor
    bme = std::make_shared<i2c::BME280>();
    auto i2c = std::make_unique<i2c::I2CController>();
    if (i2c->Init(std::make_unique<com::soc::StreamIpcSocket>()) != core::ErrorCode::kOk) {
        ara::log::LogError() << "EnvApp: Failed to initialize i2c pointer";
        return core::ErrorCode::kInitializeError;
    }
    ara::log::LogDebug() << "EnvApp: init Complete";
    if (bme->Init(std::move(i2c)) != core::ErrorCode::kOk) {
        ara::log::LogWarn() << "EnvApp: Failed to initialize BME280 sensor";
        return core::ErrorCode::kInitializeError;
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
        res = this->temp_->Initialize(529, std::bind(&EnvServiceFc::TempRxCallback,
            this, std::placeholders::_1), std::make_unique<com::soc::StreamIpcSocket>());
        ara::log::LogInfo() << "Cant connect to temp by id:529, try num: " << std::to_string(i);
        i++;
    } while (res != core::ErrorCode::kOk && i < 6);
    if (res != core::ErrorCode::kOk) {
        ara::log::LogError() << "TempController failed to initialize after retries";
        return core::ErrorCode::kInitializeError;
    }
    if (LoadTempConfig(parms) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to load temperature configuration";
        return core::ErrorCode::kInitializeError;
    }
    service_ipc.StartOffer();
    service_udp.StartOffer();
    return core::ErrorCode::kOk;
}

int EnvServiceFc::LoadTempConfig(const std::map<ara::core::StringView, ara::core::StringView>& parms) {
    ara::log::LogDebug() << "Starting function LoadTempConfig";
    const std::string path = parms.at("app_path") + "etc/config.json";
    auto parser_opt = core::json::JsonParser::Parser(path);
    ara::log::LogDebug() << path;
    if (!parser_opt.has_value()) {
        ara::log::LogError() <<("Failed to open temp_Service config file");
        return core::ErrorCode::kInitializeError;
    }
    ara::log::LogDebug() << "Opened file";
    auto temp_opt = parser_opt.value().GetArray<nlohmann::json>("sensors-temp");
    if (!temp_opt.has_value()) {
        ara::log::LogError() <<("Invalid temp_Service config format");
        return core::ErrorCode::kInitializeError;
    }
    ara::log::LogDebug() << "Loaded sensor-temp";
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
        if (!sensor_id.has_value()) {
            ara::log::LogError() << "Sensor_id is empty";
            continue;
        }
        sensorIdsToPaths[sensor_id.value()] = std::make_pair(name.value(), physical_id.value());
    }
    ara::log::LogDebug() << "Finished LoadTempConfig";
    return srp::core::ErrorCode::kOk;
}

void EnvServiceFc::Bme280Loop(const std::stop_token& token) {
     while (!token.stop_requested()) {
        const auto start = std::chrono::high_resolution_clock::now();
        const auto pressValue = this->bme->getPressure();
        const auto tempValue = this->bme->getTemperature();
        const auto humValue = this->bme->getHumidity();
        if (pressValue.has_value() && tempValue.has_value() && humValue.has_value()) {
            ara::log::LogDebug() << "Receive new Pressure: " << pressValue.value()
                                    << " temp: " << tempValue.value() << " hum: " << humValue.value();
            this->service_ipc.newBME280Event.Update({tempValue.value(), humValue.value(), pressValue.value()});
            this->service_udp.newBME280Event.Update({tempValue.value(), humValue.value(), pressValue.value()});
        } else {
            ara::log::LogWarn() << "dont receive new data";
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        ara::log::LogDebug() << "loop taken:" << std::to_string(duration.count()) << "ms";
        if (duration < std::chrono::milliseconds(kBME280_delay_ms)) {
            core::condition::wait_for(std::chrono::milliseconds(kBME280_delay_ms) - duration, token);  // 1 Hz
        }
    }
}

int EnvServiceFc::Run(const std::stop_token& token) {
    ara::log::LogDebug() << "Leci run";

    Bme280Loop(token);

    service_ipc.StopOffer();
    service_udp.StopOffer();
    return core::ErrorCode::kOk;
}

void EnvServiceFc::TempRxCallback(const std::vector<srp::mw::temp::TempReadHdr>& data) {
    auto updateEvents = [](auto& ipcEv, auto& udpEv, int16_t val) {
        ipcEv.Update(val);
        udpEv.Update(val);
    };
    for (auto &hdr : data) {
        const auto& sensorName = sensorIdsToPaths[hdr.actuator_id].first;
        const int16_t value = static_cast<int16_t>(hdr.value * 10);

        ara::log::LogDebug() << "Receive temp id: " << hdr.actuator_id
                             << ", name: " << sensorName
                             << ", temp: " << (static_cast<float>(value) / 10.0f);

        switch (sensorName) {
        case "board_temp_1":
            updateEvents(this->service_ipc.newBoardTempEvent_1, this->service_udp.newBoardTempEvent_1, value);
            break;
        case "board_temp_2":
            updateEvents(this->service_ipc.newBoardTempEvent_2, this->service_udp.newBoardTempEvent_2, value);
            break;
        case "board_temp_3":
            updateEvents(this->service_ipc.newBoardTempEvent_3, this->service_udp.newBoardTempEvent_3, value);
            break;
        default:
            ara::log::LogWarn() << "ID spoza zakresu: " << hdr.actuator_id;
            break;
        }
    }
}




}  // namespace envServiceFc
}  // namespace srp
