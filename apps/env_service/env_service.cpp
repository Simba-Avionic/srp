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
#include "apps/env_service/env_service.hpp"
#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"

namespace simba {
namespace envService {


std::vector<uint8_t> convertPayload(const double &value) {
    std::vector<uint8_t> bytes;
    bytes.resize(sizeof(int16_t));
    const int16_t floatValue = static_cast<int16_t>(value * 10);
    std::memcpy(bytes.data(), &floatValue, sizeof(int16_t));
    return bytes;
}

core::ErrorCode EnvService::Run(std::stop_token token) {
    this->SleepMainThread();
    return core::ErrorCode::kOk;
}

core::ErrorCode EnvService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    core::ErrorCode res;
    this->temp1_event = std::make_shared<com::someip::EventSkeleton>("EnvApp/newTempEvent_1");
    this->temp2_event = std::make_shared<com::someip::EventSkeleton>("EnvApp/newTempEvent_2");
    this->temp3_event = std::make_shared<com::someip::EventSkeleton>("EnvApp/newTempEvent_3");
    this->dtc_temp_error = std::make_shared<diag::dtc::DTCObject>(0x20);
    this->dtc_temp_connection_error_0xB0 = std::make_shared<diag::dtc::DTCObject>(0xB0);
    diag_controller.RegisterDTC(dtc_temp_connection_error_0xB0);
    diag_controller.RegisterDTC(dtc_temp_error);
    uint8_t i = 0;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        res = this->temp_.Init(514, std::bind(&EnvService::TempRxCallback,
            this, std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3));
    } while (res != core::ErrorCode::kOk && i < 6);
    if (res != core::ErrorCode::kOk) {
        this->dtc_temp_connection_error_0xB0->Failed();
        return res;
    }
    return core::ErrorCode::kOk;
}
bool EnvService::CheckTempError(const double &value) {
    if (value > 120 || value <-30) {
        dtc_temp_error->Failed();
        return false;
    } else {
        dtc_temp_error->Pass();
        return true;
    }
}

void EnvService::TempRxCallback(const std::string& ip, const std::uint16_t& port,
                                const std::vector<std::uint8_t> data) {
    mw::temp::TempReadingMsgFactory factory_;
    auto hdrs = factory_.GetPayload(data);
    for (auto &hdr : hdrs) {
        this->CheckTempError(hdr.second);
        AppLogger::Info("Receive temp id: "+std::to_string(hdr.first)+",temp:"+std::to_string(hdr.second));
        switch (hdr.first) {
            case 0:
            this->temp1_event->SetValue(convertPayload(hdr.second));
            break;
            case 1:
            this->temp2_event->SetValue(convertPayload(hdr.second));
            break;
            case 2:
            this->temp3_event->SetValue(convertPayload(hdr.second));
            break;
            default:
            AppLogger::Warning("ID spoza zakresu:"+std::to_string(hdr.first));
            break;
        }
    }
}


}  // namespace envService
}  // namespace simba
