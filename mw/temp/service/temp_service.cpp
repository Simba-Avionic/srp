/**
 * @file TempService.cpp
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "temp_service.h"
#include <iostream>
#include <chrono>  // NOLINT
#include <utility>  // NOLINT

#include "core/json/json_parser.h"

namespace simba {
namespace mw {
namespace temp {

namespace {
    static constexpr char const*
        kTempServiceName = "SIMBA.TEMP.SERVICE";
    static constexpr char const*
        kSubscriberPrefix = "SIMBA.TEMP.";
    constexpr uint8_t sensor_resolution = 10;
    constexpr const char* sensor_path = "/sys/bus/w1/devices/";
    constexpr auto SENSOR_TIMEOUT = 650;
}

using temp_sub_factory = simba::mw::temp::SubMsgFactory;
using temp_read_factory = simba::mw::temp::TempReadingMsgFactory;

simba::core::ErrorCode TempService::Run(std::stop_token token) {
    ConfigSensors();
    this->StartTempThread();
    AppLogger::Info("Temp Service started!");
    this->SleepMainThread();
    return core::ErrorCode::kError;
}

simba::core::ErrorCode TempService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
    LoadConfig(parms, std::make_unique<com::soc::IpcSocket>());
    if (auto ret = this->sub_sock_->Init(
        com::soc::SocketConfig(kTempServiceName, 0, 0))) {
        AppLogger::Error("Couldn't initialize " +
            std::string(kTempServiceName) + "socket!");
        return ret;
    }

    this->sub_sock_->SetRXCallback(
        std::bind(&simba::mw::temp::TempService::SubCallback, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    this->sub_sock_->StartRXThread();
    return simba::core::ErrorCode::kOk;
}

simba::core::ErrorCode TempService::ConfigSensors() const {
    for (auto sensor : this->sensorPathsToIds) {
        std::fstream file(sensor.first + "/resolution");
        if (!file) {
            AppLogger::Warning("Sensor " + sensor.first + " not available!");
            break;
        }
        AppLogger::Debug("set resolution for sensor"+sensor.first);
        file << static_cast<int>(sensor_resolution);
        file.close();
    }
    return simba::core::ErrorCode::kOk;
}

void TempService::StartTempThread() {
    if (temp_thread != nullptr) {
        AppLogger::Error("Error starting temperature thread!");
        return;
    }
    this->temp_thread = std::make_unique<std::jthread>(
        [&](std::stop_token stoken) { this->Loop(stoken); });
}

void TempService::SubCallback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t>& data) {
    auto hdr = temp_sub_factory::GetHeader(data);

    std::uint16_t service_id = hdr->GetServiceID();

    if (!this->subscribers.contains(service_id)) {
        this->subscribers.insert(service_id);
        AppLogger::Info("Registered new client with id: "
            + std::to_string(service_id));
    }
}

simba::core::ErrorCode TempService::LoadConfig(
    const std::unordered_map<std::string, std::string>& parms, std::unique_ptr<com::soc::IpcSocket> sock) {
    this->sub_sock_ = std::move(sock);
    const std::string path = "/opt/" + parms.at("app_name") + "/etc/config.json";
    auto parser_opt = core::json::JsonParser::Parser(path);
    if (!parser_opt.has_value()) {
        AppLogger::Error("Failed to open temp_Service config file");
        exit(1);
    }
    auto temp_opt = parser_opt.value().GetArray("sensors-temp");
    if (!temp_opt.has_value()) {
        AppLogger::Error("Invalid temp_Service config format");
        exit(2);
    }
    for (const auto &data : temp_opt.value()) {
        auto parser_opt = core::json::JsonParser::Parser(data);
        if (!parser_opt.has_value()) {
            continue;
        }
        auto parser = parser_opt.value();

        auto sensor_id = parser.GetNumber<uint8_t>("sensor_id");
        auto physical_id = parser.GetString("id");
        if (!sensor_id.has_value() || !physical_id.has_value()) {
            continue;
        }
        sensorPathsToIds[sensor_path+physical_id.value()] = sensor_id.value();
    }
    return simba::core::ErrorCode::kOk;
}

std::vector<TempReading> TempService::RetrieveTempReadings() const {
    std::vector<TempReading> readings;
    for (const auto& path : sensorPathsToIds) {
        std::ifstream file(path.first + "/temperature");

        if (!file) {
            AppLogger::Warning("Sensor " + path.first + " not available!");
            break;
        }

        std::string line;
        std::getline(file, line);
        const double sensorValueRaw = std::stoi(line)/ 1000.0;
        file.close();

        AppLogger::Debug("Sensor " + path.first +
            ": " + std::to_string(sensorValueRaw));

        readings.push_back(TempReading{path.second, sensorValueRaw});
    }
    return readings;
}

void TempService::SendTempReadings(
    const std::vector<TempReading>& readings) const {
    for (const auto& client_id : this->subscribers) {
        std::string ip = kSubscriberPrefix + std::to_string(client_id);

        std::vector<uint8_t> data = temp_read_factory::GetBuffer(readings);

        if (this->sub_sock_->Transmit(ip, 0, data)) {
            AppLogger::Error("Can't send message to: " + ip);
            break;
        }
    }
}

simba::core::ErrorCode TempService::Loop(std::stop_token stoken) {
    std::vector<TempReading> readings;
    while (true) {
        readings = RetrieveTempReadings();
        SendTempReadings(readings);
        readings.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(SENSOR_TIMEOUT));
    }
}

}  // namespace temp
}  // namespace mw
}  // namespace simba
