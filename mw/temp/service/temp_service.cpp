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

namespace simba {
namespace mw {
namespace temp {

simba::core::ErrorCode TempService::Run(std::stop_token token) {
    this->sub_sock_.StartRXThread();
    this->StartTempThread();
    AppLogger::Info("Temp Service started!");
    this->SleepMainThread();
    return core::ErrorCode::kError;
}

simba::core::ErrorCode TempService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
    LoadConfig(kTempConfigPath);

    if (auto ret = this->sub_sock_.Init(
        com::soc::SocketConfig(kTempServiceName, 0, 0))) {
        AppLogger::Error("Couldn't initialize " +
            std::string(kTempServiceName) + "socket!");
        return ret;
    }

    this->sub_sock_.SetRXCallback(
        std::bind(&simba::mw::temp::TempService::SubCallback, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

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
    const std::vector<std::uint8_t> data) {
    auto hdr = factory.GetHeader(data);

    std::uint16_t service_id = hdr->GetServiceID();

    if (!this->subscribers.contains(service_id)) {
        this->subscribers.insert(service_id);
        AppLogger::Info("Registered new client with id: "
            + std::to_string(service_id));
    }
}

simba::core::ErrorCode TempService::LoadConfig(const std::string& path) {
    std::ifstream file(path);

    if (!file) {
        AppLogger::Error("Couldn't load temperature sensors config!");
        return simba::core::ErrorCode::kError;
    }

    json jsonData;
    file >> jsonData;
    file.close();

    for (auto it = jsonData["sensors-temp"].begin();
        it != jsonData["sensors-temp"].end(); ++it) {
        sensorPathsToIds[it.key()] = it.value();
    }

    return simba::core::ErrorCode::kOk;
}

void TempService::RetrieveTempReadings(
    std::vector<TempReading>& readings,
    std::vector<std::future<simba::core::ErrorCode>>& futures) {
    for (const auto& path : sensorPathsToIds) {
        std::future<simba::core::ErrorCode> future =
            std::async(std::launch::async, [this, &path, &readings]
            () -> simba::core::ErrorCode {
            std::ifstream file(path.first + "/w1_slave");

            if (!file) {
                AppLogger::Warning("Sensor " + path.first + " not available!");
                return simba::core::ErrorCode::kError;
            }

            std::string line;
            double sensorValueRaw;

            while (std::getline(file, line)) {
                size_t found = line.find("t=");
                if (found != std::string::npos) {
                    sensorValueRaw = stof(line.substr(found + 2)) / 1000.0;
                    break;
                }
            }
            file.close();

            AppLogger::Debug("Sensor " + path.first +
                ": " + std::to_string(sensorValueRaw));

            readings.push_back(TempReading{path.second, sensorValueRaw});
            return simba::core::ErrorCode::kOk;
        });
        futures.push_back(std::move(future));
    }

    for (auto& future : futures) {
        future.get();
    }
    futures.clear();
}

void TempService::SendTempReadings(
    std::vector<TempReading> &readings,
    std::vector<std::future<simba::core::ErrorCode>>& futures) {
    for (const auto& client_id : this->subscribers) {
        // async functions can be deleted
        // because there won't be many sensors/clients
        std::future<simba::core::ErrorCode> future =
            std::async(std::launch::async, [this, &client_id, &readings]
            () -> simba::core::ErrorCode {
            simba::mw::temp::TempReadingMsgFactory factory;
            std::string ip = kSubscriberPrefix + std::to_string(client_id);

            std::vector<uint8_t> data =
                factory.GetBuffer(std::make_shared<TempReadingMsg>(), readings);

            if (auto ret = this->sub_sock_.Transmit(ip, 0, data)) {
                AppLogger::Error("Can't send message to: " + ip);
            }
            return simba::core::ErrorCode::kOk;
        });
        futures.push_back(std::move(future));
    }

    for (auto& future : futures) {
        future.get();
    }
    futures.clear();
}

simba::core::ErrorCode TempService::Loop(std::stop_token stoken) {
    std::vector<TempReading> readings;
    std::vector<std::future<simba::core::ErrorCode>> futures;

    while (true) {
        RetrieveTempReadings(readings, futures);
        SendTempReadings(readings, futures);
        readings.clear();
        std::this_thread::sleep_for(this->temp_timeout);
    }
}

}  // namespace temp
}  // namespace mw
}  // namespace simba
