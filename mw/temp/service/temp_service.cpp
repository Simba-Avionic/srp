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
#include <map>
#include <future>  // NOLINT
#include <algorithm>

#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "srp/mw/temp/TempReadHdr.h"
#include "srp/mw/temp/TempSubHdr.h"
namespace srp {
namespace mw {
namespace temp {

namespace {
    static constexpr auto kTempServiceName = "SRP.TEMP.SERVICE";
    static constexpr auto kSubscriberPrefix = "SRP.TEMP.";
    static constexpr auto kSensor_resolution = 10;
    static consteval uint16_t GetSensorResponseTimeMs() {
        switch (kSensor_resolution) {
        case 9:
            return 94;
        case 10:
            return 188;
        case 11:
            return 375;
        case 12:
            return 750;
        default:
            return 750;
        }
    }
}  // namespace

TempService::TempService(): did_instance{"/srp/mw/temp_service/temp_status_did"} {
    temp_driver_ = std::make_unique<core::temp::TempDriver>();
}

int TempService::Run(const std::stop_token& token) {
    std::vector<std::future<std::optional<srp::mw::temp::TempReadHdr>>> futures;
    futures.reserve(10);

    while (!token.stop_requested()) {
        core::condition::wait_for(std::chrono::milliseconds(GetSensorResponseTimeMs()), token);
        std::vector<std::pair<std::string, std::uint8_t>> sensors_snapshot;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (sensorPathsToIds.empty()) {
                continue;
            }
            sensors_snapshot.reserve(sensorPathsToIds.size());
            for (const auto& sensor : sensorPathsToIds) {
                sensors_snapshot.emplace_back(sensor.first, sensor.second);
            }
        }

        for (const auto& sensor : sensors_snapshot) {
            futures.push_back(std::async(std::launch::async,
                                [this, sensor]() -> std::optional<srp::mw::temp::TempReadHdr> {
                auto res = temp_driver_->ReadTemp(sensor.first);
                if (res.HasValue()) {
                    double val = res.ValueOr(0.00);
                    return srp::mw::temp::TempReadHdr{sensor.second, static_cast<float>(val)};
                }
                return std::nullopt;
            }));
        }

        for (auto& future : futures) {
            auto result = future.get();
            if (result.has_value()) {
                this->temp_did_->UpdateTemp(result.value().actuator_id, result.value().value);
                SendTempReading(result.value());
            }
        }
        futures.clear();
    }
    this->sub_sock_->StopRXThread();
    this->temp_did_->StopOffer();
    return 0;
}


int TempService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
    nextSensorId = 1;
    ara::log::LogInfo() << "Starting TempService Initialization";
    this->sub_sock_ = std::move(std::make_unique<com::soc::StreamIpcSocket>());
    this->sock = std::move(std::make_unique<com::soc::IpcSocket>());
    if (auto ret = this->sub_sock_->Init(
        com::soc::SocketConfig(kTempServiceName, 0, 0))) {
        ara::log::LogError() <<("Couldn't initialize " +
            std::string(kTempServiceName) + "socket!");
        return ret;
    }

    this->sub_sock_->SetRXCallback(
        [this](const std::string& ip, const uint16_t& port,
                const std::vector<std::uint8_t>& data) -> std::vector<std::uint8_t> {
            return SubCallback(ip, port, data);
        });
    this->sub_sock_->StartRXThread();
    auto sensors = temp_driver_->GetAllAccessibleSensor();
    if (!sensors.HasValue()) {
        ara::log::LogWarn() << "Cant get avaliable temp sensors";
    }
    std::stringstream ss;
    ss << "sensors: ";
    for (const auto& name : sensors.Value()) {
        ss << name << ", ";
    }
    ara::log::LogInfo() << ss.str();
    this->temp_did_ = std::make_unique<TempMWDID>(did_instance);
    this->temp_did_->Offer();
    return srp::core::ErrorCode::kOk;
}

int TempService::ConfigSensor(std::string sensorId) {
    if (!temp_driver_->SetResolution(sensorId, kSensor_resolution).HasValue()) {
        ara::log::LogWarn() <<("Unable to set resolution for sensor: " + sensorId);
    } else {
        ara::log::LogDebug() <<("Set resolution for sensor: " + sensorId);
    }
    return srp::core::ErrorCode::kOk;
}

std::vector<uint8_t> TempService::SubCallback(const std::string& ip, const uint16_t& port,
    const std::vector<std::uint8_t>& data) {
    auto hdr_opt = srp::data::Convert<srp::mw::temp::TempSubHdr>::Conv(data);
    if (!hdr_opt.has_value()) {
        return std::vector<uint8_t>{};
    }
    auto hdr = hdr_opt.value();
    uint16_t service_id = hdr.service_id;
    std::string physical_id{
        '2',
        '8',
        '-',
        hdr.physical_id_1,
        hdr.physical_id_2,
        hdr.physical_id_3,
        hdr.physical_id_4,
        hdr.physical_id_5,
        hdr.physical_id_6,
        hdr.physical_id_7,
        hdr.physical_id_8,
        hdr.physical_id_9,
        hdr.physical_id_10,
        hdr.physical_id_11,
        hdr.physical_id_12
    };
    bool new_sensor = false;
    std::uint8_t assigned_sensor_id = 0;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = sensorPathsToIds.find(physical_id);
        if (it == sensorPathsToIds.end()) {
            if (ConfigSensor(physical_id) != srp::core::ErrorCode::kOk) {
                ara::log::LogError() << "Error configuring new temp sensor";
                return std::vector<uint8_t>{};
            }
            sensorPathsToIds[physical_id] = nextSensorId;
            new_sensor = true;
            assigned_sensor_id = nextSensorId;
        } else {
            assigned_sensor_id = it->second;
        }
        subscribers[assigned_sensor_id].insert(service_id);
    }
    ara::log::LogInfo() << "Registered new client with id: "
            << service_id << " to sensor nr: " << assigned_sensor_id;

    auto res = assigned_sensor_id;
    if (new_sensor) {
        nextSensorId++;
    }
    return std::vector<uint8_t>{res};
}


void TempService::SendTempReading(const TempReadHdr& read) {
    std::set<uint16_t> subs;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        const auto it = this->subscribers.find(read.actuator_id);
        if (it == this->subscribers.end()) {
            ara::log::LogDebug() << "Can't find any subscriber for sensor: "
                                 << read.actuator_id;
            return;
        }
        if (it->second.empty()) {
            return;
        }
        subs = it->second;
    }

    std::vector<uint8_t> data = srp::data::Convert2Vector<srp::mw::temp::TempReadHdr>::Conv(read);
    for (const auto& client_id : subs) {
        std::string ip = kSubscriberPrefix + std::to_string(static_cast<int>(client_id));
        if (this->sock->Transmit(ip, 0, data)) {
            ara::log::LogError() << "Can't send message to: " << ip;
            continue;
        }
        ara::log::LogDebug() << "Sent new temp data to " << client_id;
    }
}

}  // namespace temp
}  // namespace mw
}  // namespace srp
