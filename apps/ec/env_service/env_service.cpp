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
#include <cstring>
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <sstream>
#include <iomanip>
#include "apps/ec/env_service/env_service.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"
#include "srp/env/EnvAppSkeleton.h"
#include "mw/i2c_service/controller/ads7828/controller.hpp"

namespace srp {
namespace envService {

namespace {
    static constexpr uint8_t PRESS_SENSOR_ID =           10;
    static constexpr uint8_t D_PRESS_SENSOR_ID =         11;
    static constexpr auto kPressureDelayMs =             100;
    static constexpr auto kDifferentialPressureDelayMs = 100;
    static constexpr auto kPressure_sensor_multiplicator = 100;
}  // namespace



core::ErrorCode EnvService::Init(std::unique_ptr<mw::temp::TempController> temp) {
    if (this->temp_ || !temp) {
      return core::ErrorCode::kInitializeError;
    }
    this->temp_ = std::move(temp);
    if (config.Init() != core::ErrorCode::kOk) {
      ara::log::LogError() << "EnvService::Init: EEPROM ConfigManager init failed";
      return core::ErrorCode::kInitializeError;
    }
    return core::ErrorCode::kOk;
}

EnvService::EnvService(): press_{std::move(std::make_shared<i2c::ADCSensorController>())},
                service_ipc{ara::core::InstanceSpecifier{"srp/env/EnvApp/envService_ipc"}},
                service_udp{ara::core::InstanceSpecifier{"srp/env/EnvApp/envService_udp"}} {
}


int EnvService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
    auto app_path_it = parms.find("app_path");
    if (app_path_it == parms.end()) {
        ara::log::LogError() << "app_path parameter not found in parms";
        return core::ErrorCode::kInitializeError;
    }

    auto adc = std::make_unique<i2c::ADS7828>();
    auto i2c = std::make_unique<i2c::I2CController>();
    i2c->Init(std::make_unique<com::soc::StreamIpcSocket>());
    auto adc_init_res = adc->Init(std::move(i2c));
    if (adc_init_res != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize ADC";
        return core::ErrorCode::kInitializeError;
    }

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
    if (res != core::ErrorCode::kOk) {
        ara::log::LogError() << "TempController failed to initialize after retries";
        return core::ErrorCode::kInitializeError;
    }
    if (LoadTempConfig(parms) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to load temperature configuration";
        return core::ErrorCode::kInitializeError;
    }
    const std::optional<eeprom::EEPROM_config> eeprom_cfg = config.GetConfig();
    if (!eeprom_cfg.has_value()) {
        ara::log::LogError() << "Failed to load EEPROM temperature configuration";
        return core::ErrorCode::kInitializeError;
    }
    auto register_sensor = [&](const std::string& raw_id, const std::string& label) -> core::ErrorCode {
        std::string physical_id = "28-" + raw_id;
        auto sensor_id = this->temp_->Register(physical_id);

        if (!sensor_id.has_value()) {
            ara::log::LogError() << "Sensor_id is empty for " << label;
            return core::ErrorCode::kInitializeError;
        }

        sensorIdsToPaths[sensor_id.value()] = std::make_pair(label, physical_id);
        return core::ErrorCode::kOk;
    };
    if (register_sensor(eeprom_cfg.value().board_temp1_id, "board_1") !=
                                            core::ErrorCode::kOk) return core::ErrorCode::kInitializeError;
    if (register_sensor(eeprom_cfg.value().board_temp2_id, "board_2") !=
                                            core::ErrorCode::kOk) return core::ErrorCode::kInitializeError;
    if (register_sensor(eeprom_cfg.value().board_temp3_id, "board_3") !=
                                            core::ErrorCode::kOk) return core::ErrorCode::kInitializeError;

    // eeprom::EEPROM_config cfg{};
    // cfg.pca9685_XO_corelation = 1.038;
    // strncpy(cfg.board_temp1_id, "00001110ff83", sizeof(cfg.board_temp1_id));
    // strncpy(cfg.board_temp2_id, "00001110e48e", sizeof(cfg.board_temp2_id));
    // strncpy(cfg.board_temp3_id, "0000107b3c59", sizeof(cfg.board_temp3_id));
    // if (config.SetConfig(cfg) != core::ErrorCode::kOk) {
    //     ara::log::LogError() << "cant set eeprom";
    // } else {
    //     auto read_back = config.GetConfig();
    //     if (!read_back.has_value()) {
    //         ara::log::LogError() << "EEPROM read-back failed after SetConfig";
    //     } else if (std::memcmp(&read_back.value(), &cfg, sizeof(cfg)) != 0) {
    //         ara::log::LogError() << "EEPROM read-back mismatch after SetConfig";
    //     } else {
    //         ara::log::LogInfo() << "EEPROM SetConfig + GetConfig roundtrip OK";
    //     }
    // }
    return core::ErrorCode::kOk;
}

core::ErrorCode EnvService::LoadTempConfig(const std::map<ara::core::StringView, ara::core::StringView>& parms) {
    ara::log::LogInfo() << "Starting function LoadTempConfig";
    const std::string path = parms.at("app_path") + "etc/config.json";
    auto parser_opt = core::json::JsonParser::Parser(path);
    ara::log::LogInfo() << path;
    if (!parser_opt.has_value()) {
        ara::log::LogError() << "Failed to open temp_Service config file";
        return core::ErrorCode::kInitializeError;
    }
    ara::log::LogInfo() << "Opened file";
    auto temp_opt = parser_opt.value().GetArray<nlohmann::json>("sensors-temp");
    if (!temp_opt.has_value()) {
        ara::log::LogError() << "Invalid temp_Service config format";
        return core::ErrorCode::kInitializeError;
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
        if (!sensor_id.has_value()) {
            ara::log::LogError() << "Sensor_id is empty";
            continue;
        }
        sensorIdsToPaths[sensor_id.value()] = std::make_pair(name.value(), physical_id.value());
    }
    return core::ErrorCode::kOk;
}

void EnvService::GenericPressureLoop(
            const std::stop_token& token,
            uint8_t sensorId,
            std::chrono::milliseconds delay,
            const std::string& label,
            auto& eventIpc,
            auto& eventUdp) {
    while (!token.stop_requested()) {
        auto start = std::chrono::high_resolution_clock::now();
        std::optional<float> pressValue;
        {
            std::lock_guard<std::mutex> lock(press_mtx_);
            pressValue = this->press_->GetValue(sensorId);
        }

        if (pressValue.has_value()) {
            float val = pressValue.value();

            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << val;
            ara::log::LogDebug() << "Receive new " << label << ": " << ss.str() << " Bar";

            uint16_t encodedVal = static_cast<uint16_t>(val * kPressure_sensor_multiplicator);
            eventIpc.Update(encodedVal);
            service_udp.SetTankPressure(encodedVal);
            eventUdp.Update(encodedVal);
        } else {
            ara::log::LogWarn() << "Don't receive new " << label;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        ara::log::LogDebug() << label << " loop taken: " << duration.count() << "ms";

        if (duration < delay) {
            core::condition::wait_for(delay - duration, token);
        }
    }
}

int EnvService::Run(const std::stop_token& token) {
    service_ipc.StartOffer();
    service_udp.StartOffer();
    temp_->StartRxThread();
    std::jthread pressure_thread([this, token] {
        GenericPressureLoop(token, PRESS_SENSOR_ID,
                            std::chrono::milliseconds(kPressureDelayMs),
                            "Tank Pressure",
                            service_ipc.newPressEvent,
                            service_udp.newPressEvent);
    });

    std::jthread differential_pressure_thread([this, token] {
        GenericPressureLoop(token, D_PRESS_SENSOR_ID,
                            std::chrono::milliseconds(kDifferentialPressureDelayMs),
                            "Tank D Pressure",
                            service_ipc.newDPressEvent,
                            service_udp.newDPressEvent);
    });
    core::condition::wait(token);

    service_ipc.StopOffer();
    service_udp.StopOffer();
    return core::ErrorCode::kOk;
}

void EnvService::TempRxCallback(const std::vector<srp::mw::temp::TempReadHdr>& data) {
    for (auto &hdr : data) {
        auto pathIt = sensorIdsToPaths.find(hdr.actuator_id);
        if (pathIt == sensorIdsToPaths.end()) {
            ara::log::LogWarn() << "Unknown sensor id: " << hdr.actuator_id;
            continue;
        }
        const auto& sensorName = pathIt->second.first;
        const int16_t value = static_cast<int16_t>(hdr.value * 10);

        ara::log::LogDebug() << "Receive new temp id: " << hdr.actuator_id
                            << ", name: " << sensorName << ", temp: " << hdr.value;

        using UpdateFn = std::function<void(int16_t)>;
        static const std::unordered_map<std::string, UpdateFn> eventMap = {
            {"sensor_temp_1", [this](int16_t v) {
                service_ipc.newTempEvent_1.Update(v);
                service_udp.newTempEvent_1.Update(v);
                service_udp.SetUpTankTemp(v);
            }},
            {"sensor_temp_2", [this](int16_t v) {
                service_ipc.newTempEvent_2.Update(v);
                service_udp.newTempEvent_2.Update(v);
                service_udp.SetDownTankTemp(v);
            }},
            {"sensor_temp_3", [this](int16_t v) {
                service_ipc.newTempEvent_3.Update(v);
                service_udp.newTempEvent_3.Update(v);
            }},
            {"board_1",       [this](int16_t v) {
                service_ipc.newBoardTempEvent1.Update(v);
                service_udp.newBoardTempEvent1.Update(v);
            }},
            {"board_2",       [this](int16_t v) {
                service_ipc.newBoardTempEvent2.Update(v);
                service_udp.newBoardTempEvent2.Update(v);
            }},
            {"board_3",       [this](int16_t v) {
                service_ipc.newBoardTempEvent3.Update(v);
                service_udp.newBoardTempEvent3.Update(v);
            }}
        };

        auto it = eventMap.find(sensorName);
        if (it != eventMap.end()) {
            it->second(value);
        } else {
            ara::log::LogWarn() << "No mapping for sensor name: " << sensorName
                                << " (id=" << hdr.actuator_id << ")";
        }
    }
}




}  // namespace envService
}  // namespace srp
