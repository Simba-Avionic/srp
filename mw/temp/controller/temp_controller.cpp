/**
 * @file temp_controller.cpp
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "temp_controller.h"
#include <iostream>

namespace simba {
namespace mw {
namespace temp {

simba::core::ErrorCode TempController::Init(uint16_t service_id) {
    this->service_id = service_id;

    if (auto ret = this->sub_sock_.Init(
        com::soc::SocketConfig(
            kSubscriberPrefix + std::to_string(this->service_id), 0, 0))) {
        AppLogger::Error("Couldn't initialize socket!");
        return ret;
    }
    this->sub_sock_.SetRXCallback(
        std::bind(&simba::mw::temp::TempController::Callback, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    this->Subscribe();
    return simba::core::ErrorCode::kOk;
}

simba::core::ErrorCode TempController::Subscribe() {
    simba::mw::temp::SubMsgFactory factory;
    SubscribeHeader hdr{this->service_id};
    std::vector<uint8_t> data =
        factory.GetBuffer(std::make_shared<SubscribeHeader>(hdr), {});

    if (auto ret = sub_sock_.Transmit(kTempServiceName, 0, data)) {
        AppLogger::Error("Failed to subscribe to " + std::string(kTempServiceName));
        return ret;
    }
    return simba::core::ErrorCode::kOk;
}

void TempController::Callback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t> data) {
    this->latest_readings = factory.GetPayload(data);

    for (auto reading : latest_readings) {
        AppLogger::Debug(std::to_string(reading.first) +
                " " + std::to_string(reading.second));
    }
    return;
}

}  // namespace temp
}  // namespace mw
}  // namespace simba