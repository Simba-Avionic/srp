#include "TempController.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <future>

namespace simba {
namespace mw {
namespace temp {

simba::core::ErrorCode TempController::Run(std::stop_token token) {

    this->sub_sock_.StartRXThread();
    this->StartTempThread();
    std::cout << "Started TempController!" << std::endl;

    this->SleepMainThread();

    return core::ErrorCode::kError;
}

simba::core::ErrorCode TempController::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {

    LoadConfig(kTempConfigPath);

    if (auto ret = this->sub_sock_.Init(
        com::soc::SocketConfig(kTempControllerName, 0, 0)))
    {
        AppLogger::Error("Couldn't initialize " + 
                std::string(kTempControllerName) + "socket!");
        return ret; 
    }

    // TODO: get path_to_id from config
    
    this->sub_sock_.SetRXCallback(
        std::bind(&simba::mw::temp::TempController::SubCallback, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    return simba::core::ErrorCode::kOk;
}

void TempController::StartTempThread() {
    if (temp_thread != nullptr) {
        AppLogger::Error("Error starting temperature thread!");
        return;
    }
    this->temp_thread = std::make_unique<std::jthread>(
        [&](std::stop_token stoken) { this->SendTempData(stoken); });
}

void TempController::SubCallback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t> data)
{
    // TODO: register clients
    simba::mw::temp::SubMsgFactory factory;
    auto hdr = factory.GetHeader(data);
    // std::vector<uint8_t> payload = factory.GetPayload(data);

    std::cout << "Service want to subscribe:" << hdr.get()->GetServiceID() << std::endl; 

    // this->diag_controller->Write(
    //     0x0201, 0x0001, this->conv_.convertUint16ToVector(hdr->GetDtcID()));
}

simba::core::ErrorCode TempController::LoadConfig(const std::string& path) {

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

simba::core::ErrorCode TempController::SendTempData(std::stop_token stoken) {

    std::vector<TempReading> readings;
    std::vector<std::future<simba::core::ErrorCode>> futures;

    while (true) {

        for (const auto& path : sensorPathsToIds)
        {
            std::future<simba::core::ErrorCode> future = 
                std::async(std::launch::async, [this, &path, &readings]
                () -> simba::core::ErrorCode
            {
                std::ifstream file(path.first + "/temperature");
                
                if (!file) {
                    AppLogger::Warning("Sensor " + path.first + " not available!");
                    return simba::core::ErrorCode::kError;
                }

                _Float64 sensorValue;
                file >> sensorValue;
                file.close();

                readings.push_back(TempReading{path.second, sensorValue/1000.0});
            });
            futures.push_back(std::move(future));
        }

        for (auto& future : futures) {
            future.get();
        }
        futures.clear();

        for (const auto& client : this->subscribers)
        {
            std::future<simba::core::ErrorCode> future = 
                std::async(std::launch::async, [this, &client, &readings]
                () -> simba::core::ErrorCode
            {
                // TODO: serialize payload and client
                this->sub_sock_.Transmit(
                    "Engine." + std::to_string(client), 0, {0});
            });
            futures.push_back(std::move(future));
        }

        for (auto& future : futures) {
            future.get();
        }
        futures.clear();
        readings.clear();
        std::this_thread::sleep_for(this->temp_timeout);
    }

}

// TODO: REGISTER CLIENTS (SUBSCRIBERS)
// we've got a socket for service and in the callback function
// it should create user and send data to it

// TODO: SEND DATA TO CLIENTS
// need a way to store and retrieve data from sensors

// TODO: GET DATA FROM TEMPERATURE SENSORS
// from TempController idk yet how - another socket maybe

} // namespace temp
} // namespace mw
} // namespace simba