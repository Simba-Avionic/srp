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
    constexpr uint8_t PRESS_SENSOR_ID = 10;
}


core::ErrorCode EnvService::Init(std::unique_ptr<mw::temp::TempController> temp) {
    if (this->temp_ || !temp) {
      return core::ErrorCode::kInitializeError;
    }
    this->temp_ = std::move(temp);
    return core::ErrorCode::kOk;
}

EnvService::EnvService(): service_ipc{ara::core::InstanceSpecifier{"srp/env/EnvApp/envService_ipc"}},
                        service_udp{ara::core::InstanceSpecifier{"srp/env/EnvApp/envService_udp"}} {
}


int EnvService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
    // Check if app_path exists in parameters
    auto app_path_it = parms.find("app_path");
    if (app_path_it == parms.end()) {
        ara::log::LogError() << "app_path parameter not found in parms";
        return core::ErrorCode::kInitializeError;
    }

    // Initialize pressure sensor
    auto adc = std::make_unique<i2c::ADS7828>();
    auto i2c = std::make_unique<i2c::I2CController>();
    i2c->Init(std::make_unique<com::soc::StreamIpcSocket>());
    auto adc_init_res = adc->Init(std::move(i2c));
    if (adc_init_res != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize ADC";
        return core::ErrorCode::kInitializeError;
    }

    // Convert StringView to string for Init
    std::string app_path_str(app_path_it->second.data(), app_path_it->second.size());
    auto press_init_res = this->press_.Init(app_path_str, std::move(adc));
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
            this, std::placeholders::_1), std::make_unique<com::soc::IpcSocket>());
        i++;
    } while (res != core::ErrorCode::kOk && i < 6);
    service_ipc.StartOffer();
    service_udp.StartOffer();
    return core::ErrorCode::kOk;
}

int EnvService::Run(const std::stop_token& token) {
    while (!token.stop_requested()) {
        core::condition::wait_for(std::chrono::milliseconds(1000), token);
        auto pressValue = this->press_.GetValue(PRESS_SENSOR_ID);
        if (pressValue.has_value()) {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(1) << pressValue.value();
           ara::log::LogDebug() << "Receive new Tank Pressure: " << ss.str() << " Bar";
            this->service_ipc.newPressEvent.Update(pressValue.value());
            this->service_udp.newPressEvent.Update(pressValue.value());
        } else {
            ara::log::LogWarn() << "dont receive new pressure";
        }
    }
    service_ipc.StopOffer();
    service_udp.StopOffer();
    return core::ErrorCode::kOk;
}

void EnvService::TempRxCallback(const std::vector<srp::mw::temp::TempReadHdr>& data) {
    for (auto &hdr : data) {
        const int16_t value = static_cast<int16_t>(hdr.value * 10);
        ara::log::LogInfo() << "Receive temp id: " << hdr.actuator_id << ", temp: " << static_cast<float>(value/10);
        switch (hdr.actuator_id) {
            case 0:
                this->service_ipc.newTempEvent_1.Update(value);
                this->service_udp.newTempEvent_1.Update(value);
                break;
            case 1:
                this->service_ipc.newTempEvent_2.Update(value);
                this->service_udp.newTempEvent_2.Update(value);
                break;
            case 2:
                this->service_ipc.newTempEvent_3.Update(value);
                this->service_udp.newTempEvent_3.Update(value);
                break;
            default:
                ara::log::LogWarn() << "ID spoza zakresu:" << hdr.actuator_id;
            break;
        }
    }
}


}  // namespace envService
}  // namespace srp
