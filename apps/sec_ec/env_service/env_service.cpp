/**
 * @file env_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Implementation of secondary EC environment service
 * @version 0.1
 * @date 2026-07-01
 *
 * @copyright Copyright (c) 2026
 */
#include "apps/sec_ec/env_service/env_service.hpp"

#include <cstring>
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <sstream>
#include <iomanip>

#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "ara/log/log.h"
#include "mw/i2c_service/controller/ads7828/controller.hpp"

namespace srp {
namespace sec_ec {

namespace {
  static constexpr uint8_t kEthanolPressSensorId = 14;
  static constexpr uint8_t kChamberPress2SensorId = 13;
  static constexpr uint8_t kChamberPress3SensorId = 15;
  static constexpr auto kPressureDelayMs = 50;
}  // namespace

core::ErrorCode SecEnvService::Init(std::unique_ptr<mw::temp::TempController> temp) {
    if (this->temp_ || !temp) {
      return core::ErrorCode::kInitializeError;
    }
    this->temp_ = std::move(temp);
    if (config.Init() != core::ErrorCode::kOk) {
      ara::log::LogError() << "SecEnvService::Init: EEPROM ConfigManager init failed";
      return core::ErrorCode::kInitializeError;
    }
    return core::ErrorCode::kOk;
}

SecEnvService::SecEnvService()
    : press_{std::make_shared<i2c::ADCSensorController>()},
      service_ipc{ara::core::InstanceSpecifier{"srp/env/secEnvApp/SecEnvApp_ipc"}},
      service_udp{ara::core::InstanceSpecifier{"srp/env/secEnvApp/SecEnvApp_udp"}} {}

int SecEnvService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
    auto app_path_it = parms.find("app_path");
    if (app_path_it == parms.end()) {
        ara::log::LogError() << "SecEnvService::Initialize: app_path parameter not found";
        return core::ErrorCode::kInitializeError;
    }

    auto adc = std::make_unique<i2c::ADS7828>();
    auto i2c = std::make_unique<i2c::I2CController>();
    const auto i2c_error_code = i2c->Init(std::make_unique<com::soc::StreamIpcSocket>());
    if (i2c_error_code != core::ErrorCode::kOk) {
        ara::log::LogError() << "SecEnvService::Initialize: failed to initialize I2C";
        return core::ErrorCode::kInitializeError;
    }
    auto adc_init_res = adc->Init(std::move(i2c));
    if (adc_init_res != core::ErrorCode::kOk) {
        ara::log::LogError() << "SecEnvService::Initialize: failed to initialize ADC";
        return core::ErrorCode::kInitializeError;
    }

    std::string app_path_str(app_path_it->second.data(), app_path_it->second.size());
    auto press_init_res = this->press_->Init(app_path_str, std::move(adc));
    if (press_init_res != core::ErrorCode::kOk) {
        ara::log::LogError() << "SecEnvService::Initialize: failed to initialize pressure controller";
        return core::ErrorCode::kInitializeError;
    }

    auto temp_init_res = this->Init(std::make_unique<mw::temp::TempController>());
    if (temp_init_res != core::ErrorCode::kOk) {
        ara::log::LogError() << "SecEnvService::Initialize: failed to initialize TempController";
        return core::ErrorCode::kInitializeError;
    }

    core::ErrorCode res;
    uint8_t i = 0;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        res = this->temp_->Initialize(514, std::bind(&SecEnvService::TempRxCallback,
            this, std::placeholders::_1), std::make_unique<com::soc::StreamIpcSocket>());
        i++;
    } while (res != core::ErrorCode::kOk && i < 6);
    if (res != core::ErrorCode::kOk) {
        ara::log::LogError() << "SecEnvService::Initialize: TempController init failed after retries";
        return core::ErrorCode::kInitializeError;
    }

    if (LoadTempConfig(parms) != core::ErrorCode::kOk) {
        ara::log::LogError() << "SecEnvService::Initialize: failed to load temperature configuration";
        return core::ErrorCode::kInitializeError;
    }

    const std::optional<eeprom::EEPROM_config> eeprom_cfg = config.GetConfig();
    if (!eeprom_cfg.has_value()) {
        ara::log::LogError() << "SecEnvService::Initialize: failed to load EEPROM configuration";
        return core::ErrorCode::kInitializeError;
    }

    auto register_sensor = [&](const char* raw_id, std::size_t raw_id_max_len,
                               const std::string& label) -> core::ErrorCode {
        const std::string raw_id_str(raw_id, strnlen(raw_id, raw_id_max_len));
        const std::string physical_id = "28-" + raw_id_str;
        const auto sensor_id = this->temp_->Register(physical_id);

        if (!sensor_id.has_value()) {
            ara::log::LogError() << "SecEnvService::Initialize: sensor_id is empty for " << label;
            return core::ErrorCode::kInitializeError;
        }

        sensorIdsToPaths[sensor_id.value()] = std::make_pair(label, physical_id);
        return core::ErrorCode::kOk;
    };

    if (register_sensor(eeprom_cfg.value().board_temp1_id,
                        sizeof(eeprom_cfg.value().board_temp1_id), "board_1") !=
        core::ErrorCode::kOk) {
        return core::ErrorCode::kInitializeError;
    }
    if (register_sensor(eeprom_cfg.value().board_temp2_id,
                        sizeof(eeprom_cfg.value().board_temp2_id), "board_2") !=
        core::ErrorCode::kOk) {
        return core::ErrorCode::kInitializeError;
    }
    if (register_sensor(eeprom_cfg.value().board_temp3_id,
                        sizeof(eeprom_cfg.value().board_temp3_id), "board_3") !=
        core::ErrorCode::kOk) {
        return core::ErrorCode::kInitializeError;
    }

    return core::ErrorCode::kOk;
}

core::ErrorCode SecEnvService::LoadTempConfig(
    const std::map<ara::core::StringView, ara::core::StringView>& parms) {
    const auto app_path_it = parms.find("app_path");
    if (app_path_it == parms.end()) {
        ara::log::LogError() << "SecEnvService::LoadTempConfig: app_path parameter not found";
        return core::ErrorCode::kInitializeError;
    }
    const std::string path =
        std::string(app_path_it->second.data(), app_path_it->second.size()) + "etc/config.json";
    auto parser_opt = core::json::JsonParser::Parser(path);
    if (!parser_opt.has_value()) {
        ara::log::LogError() << "SecEnvService::LoadTempConfig: failed to open config file";
        return core::ErrorCode::kInitializeError;
    }
    auto temp_opt = parser_opt.value().GetArray<nlohmann::json>("sensors-temp");
    if (!temp_opt.has_value()) {
        return core::ErrorCode::kOk;
    }
    for (const auto& data : temp_opt.value()) {
        auto entry_opt = core::json::JsonParser::Parser(data);
        if (!entry_opt.has_value()) {
            continue;
        }
        auto parser = entry_opt.value();
        auto physical_id = parser.GetString("physical_id");
        auto name = parser.GetString("name");
        if (!physical_id.has_value() || !name.has_value()) {
            continue;
        }

        std::optional<uint8_t> sensor_id = this->temp_->Register(physical_id.value());
        if (!sensor_id.has_value()) {
            ara::log::LogError() << "SecEnvService::LoadTempConfig: sensor_id is empty";
            continue;
        }
        sensorIdsToPaths[sensor_id.value()] = std::make_pair(name.value(), physical_id.value());
    }
    return core::ErrorCode::kOk;
}

void SecEnvService::GenericPressureLoop(
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
            ara::log::LogInfo() << "SecEnvService: new " << label << ": " << ss.str() << " Bar";
            const auto fixed_val = static_cast<int16_t>(val * 100);
            {
                std::lock_guard lock(service_mtx_);
                eventIpc.Update(fixed_val);
                eventUdp.Update(fixed_val);
            }
        } else {
            ara::log::LogWarn() << "SecEnvService: no new " << label;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        if (duration < delay) {
            core::condition::wait_for(delay - duration, token);
        }
    }
}

int SecEnvService::Run(const std::stop_token& token) {
    service_ipc.StartOffer();
    service_udp.StartOffer();
    temp_->StartRxThread();

    std::jthread ethanol_press_thread([this, token] {
        GenericPressureLoop(token, kEthanolPressSensorId,
                            std::chrono::milliseconds(kPressureDelayMs),
                            "Ethanol Pressure",
                            service_ipc.newEthanolPressEvent,
                            service_udp.newEthanolPressEvent);
    });

    std::jthread chamber_press2_thread([this, token] {
        GenericPressureLoop(token, kChamberPress2SensorId,
                            std::chrono::milliseconds(kPressureDelayMs),
                            "Chamber Pressure 2",
                            service_ipc.newChamberPressEvent2,
                            service_udp.newChamberPressEvent2);
    });

    std::jthread chamber_press3_thread([this, token] {
        GenericPressureLoop(token, kChamberPress3SensorId,
                            std::chrono::milliseconds(kPressureDelayMs),
                            "Chamber Pressure 3",
                            service_ipc.newChamberPressEvent3,
                            service_udp.newChamberPressEvent3);
    });

    core::condition::wait(token);

    service_ipc.StopOffer();
    service_udp.StopOffer();
    return core::ErrorCode::kOk;
}

void SecEnvService::TempRxCallback(const std::vector<srp::mw::temp::TempReadHdr>& data) {
    for (auto& hdr : data) {
        auto pathIt = sensorIdsToPaths.find(hdr.actuator_id);
        if (pathIt == sensorIdsToPaths.end()) {
            ara::log::LogWarn() << "SecEnvService: unknown sensor id: " << hdr.actuator_id;
            continue;
        }
        const auto& sensorName = pathIt->second.first;
        const int16_t value = static_cast<int16_t>(hdr.value * 10);

        using UpdateFn = std::function<void(int16_t)>;
        static const std::unordered_map<std::string, UpdateFn> eventMap = {
            {"board_1", [this](int16_t v) {
                service_ipc.newBoardTempEvent1.Update(v);
                service_udp.newBoardTempEvent1.Update(v);
            }},
            {"board_2", [this](int16_t v) {
                service_ipc.newBoardTempEvent2.Update(v);
                service_udp.newBoardTempEvent2.Update(v);
            }},
            {"board_3", [this](int16_t v) {
                service_ipc.newBoardTempEvent3.Update(v);
                service_udp.newBoardTempEvent3.Update(v);
            }},
        };

        auto it = eventMap.find(sensorName);
        if (it != eventMap.end()) {
            it->second(value);
        } else {
            ara::log::LogWarn() << "SecEnvService: no mapping for sensor name: " << sensorName
                                << " (id=" << hdr.actuator_id << ")";
        }
    }
}

}  // namespace sec_ec
}  // namespace srp
