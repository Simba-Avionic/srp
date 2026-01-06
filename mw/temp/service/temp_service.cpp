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
    static constexpr char const*
        kTempServiceName = "SRP.TEMP.SERVICE";
    static constexpr char const*
        kSubscriberPrefix = "SRP.TEMP.";
    constexpr uint8_t kSensor_resolution = 10;
    constexpr auto kSensor_Delay = 750;
    constexpr uint16_t kDefault_Response_Time = 125;
}


TempService::TempService(): did_instance{"/srp/mw/temp_service/temp_status_did"} {
    temp_driver_ = std::make_unique<core::temp::TempDriver>();
    delay_time = 0;
}

int TempService::Run(const std::stop_token& token) {
    ConfigSensors();
    std::vector<srp::mw::temp::TempReadHdr> readings;
    while (!token.stop_requested()) {
        readings = RetrieveTempReadings();
        for (const auto& read : readings) {
            this->temp_did_->UpdateTemp(read.actuator_id, read.value);
        }
        SendTempReadings(readings);
        readings.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(kSensor_Delay-kDefault_Response_Time));
    }
    this->sub_sock_->StopRXThread();
    this->temp_did_->StopOffer();
    return 0;
}

int TempService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
    ara::log::LogInfo() << "Starting TempService Initialization";
    this->sub_sock_ = std::move(std::make_unique<com::soc::StreamIpcSocket>());
    if (auto ret = this->sub_sock_->Init(
        com::soc::SocketConfig(kTempServiceName, 0, 0))) {
        ara::log::LogError() <<("Couldn't initialize " +
            std::string(kTempServiceName) + "socket!");
        return ret;
    }

    this->sub_sock_->SetRXCallback(
        std::bind(&srp::mw::temp::TempService::SubCallback, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    this->sub_sock_->StartRXThread();
    std::vector<uint8_t> sensors_id;
    for (const auto& id : sensorPathsToIds) {
        sensors_id.push_back(id.second);
    }
    this->temp_did_ = std::make_unique<TempMWDID>(did_instance, sensors_id);
    this->temp_did_->Offer();
    return srp::core::ErrorCode::kOk;
}

int TempService::ConfigSensors() {
    for (const auto& sensor : this->sensorPathsToIds) {
        if (!temp_driver_->SetResolution(sensor.first, kSensor_resolution).HasValue()) {
            ara::log::LogDebug() <<("INVALID TO SET RESOLUTION FOR "+sensor.first);
        } else {
            ara::log::LogDebug() <<("set resolution for sensor"+sensor.first);
        }
    }
    for (const auto& sensor : sensorPathsToIds) {
        auto res = temp_driver_->GetResponseTime(sensor.first);
        this->delay_time = res.ValueOr(kDefault_Response_Time);
        break;
    }
    return srp::core::ErrorCode::kOk;
}

std::vector<uint8_t> TempService::SubCallback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t>& data) {
    auto hdr = srp::data::Convert<srp::mw::temp::TempSubHdr>::Conv(data);
    if (!hdr.has_value()) {
        return std::vector<uint8_t>{};
    }
    std::uint16_t service_id = hdr.value().service_id;
    std::string physical_id{
        '2',
        '8',
        '-',
        hdr.value().physical_id_1,
        hdr.value().physical_id_2,
        hdr.value().physical_id_3,
        hdr.value().physical_id_4,
        hdr.value().physical_id_5,
        hdr.value().physical_id_6,
        hdr.value().physical_id_7,
        hdr.value().physical_id_8,
        hdr.value().physical_id_9,
        hdr.value().physical_id_10,
        hdr.value().physical_id_11,
        hdr.value().physical_id_12
    };


    if (!this->sensorPathsToIds.count(physical_id)) {
        this->sensorPathsToIds[physical_id] = nextSensorId;
        ara::log::LogInfo() << "Registered new sensor with id: " <<
                        physical_id << " as " << std::to_string(nextSensorId);
        this->subscribers[nextSensorId].insert(service_id);
        ara::log::LogInfo() <<("Registered new client with id: "
            + std::to_string(service_id) + " to sensor nr: " + std::to_string(this->sensorPathsToIds[physical_id]));
    } else if (!this->subscribers[this->sensorPathsToIds[physical_id]].contains(service_id)) {
        this->subscribers[this->sensorPathsToIds[physical_id]].insert(service_id);
        ara::log::LogInfo() << "Registered new client with id: " <<
                    std::to_string(service_id) << " to sensor nr: " <<
                    std::to_string(this->sensorPathsToIds[physical_id]);
    }
    return std::vector<uint8_t>{nextSensorId++};
}


std::vector<srp::mw::temp::TempReadHdr> TempService::RetrieveTempReadings() const {
    std::vector<std::future<std::optional<srp::mw::temp::TempReadHdr>>> futures;
    for (const auto& sensor : sensorPathsToIds) {
        futures.push_back(std::async(std::launch::async,
                            [this, &sensor]() -> std::optional<srp::mw::temp::TempReadHdr> {
            auto res = temp_driver_->ReadTemp(sensor.first);
            if (res.HasValue()) {
                double val = res.ValueOr(0.00);
                return srp::mw::temp::TempReadHdr{sensor.second, static_cast<float>(val)};
            }
            return std::nullopt;
        }));
    }
    std::vector<srp::mw::temp::TempReadHdr> readings;
    for (auto& future : futures) {
        auto result = future.get();
        if (result.has_value()) {
            readings.push_back(result.value());
        }
    }
    return readings;
}
std::vector<uint8_t> TempService::Conv(const std::vector<srp::mw::temp::TempReadHdr>& readings) const {
    std::vector<uint8_t> out;
    for (const auto read : readings) {
        auto part = srp::data::Convert2Vector<srp::mw::temp::TempReadHdr>::Conv(read);
        out.insert(out.end(), part.begin(), part.end());
    }
    return out;
}


void TempService::SendTempReadings(const std::vector<srp::mw::temp::TempReadHdr>& readings) const {
    for (const auto& read : readings) {
        auto it = this->subscribers.find(read.actuator_id);
        if (it == this->subscribers.end()) {
            ara::log::LogDebug() << "Can't find any subscriber for sensor: " <<
                                std::to_string(static_cast<int>(read.actuator_id));
            continue;
        }
        for (const auto& client_id : it->second) {
            std::string ip = kSubscriberPrefix + std::to_string(static_cast<int>(client_id));
            std::vector<uint8_t> data = srp::data::Convert2Vector<srp::mw::temp::TempReadHdr>::Conv(read);
            if (this->sub_sock_->Transmit(ip, 0, data)) {
                ara::log::LogError() << ("Can't send message to: " + ip);
                continue;
            }
        }
    }
}

}  // namespace temp
}  // namespace mw
}  // namespace srp
