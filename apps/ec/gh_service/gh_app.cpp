/**
 * @file gh_service.cpp
 * @author Grzegorz Horbaczewski (gregority9@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "apps/ec/gh_service/gh_app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"
#include <sys/resource.h>
#include "core/csvdriver/csvdriver.h"

namespace {
    constexpr uint8_t FORCE_SENSOR_CHANNEL = 3;
    constexpr auto LOOP_TIME_MS = 1;
}


namespace srp {
namespace apps {

MyGhApp::MyGhApp(): service_ipc{ara::core::InstanceSpecifier{"srp/apps/GhApp/ghService_ipc"}},
                    service_udp{ara::core::InstanceSpecifier{"srp/apps/GhApp/ghService_udp"}} {
}


int MyGhApp::Run(const std::stop_token& token) {
    uint64_t i, j = 0;
    ara::log::LogInfo() << "Running GH Application";
    auto boot = std::chrono::high_resolution_clock::now();
    auto csv = csv::CSVDriver();
    csv.Init(std::make_unique<core::FileHandler>());
    if (csv.Open("/home/root/test.csv", "TIMESTAMP;VALUE") != 0 ) {
        ara::log::LogWarn() << "Cant create file";
    }
    while (!token.stop_requested()) {
        i++;
        auto start = std::chrono::high_resolution_clock::now();
        auto forceValue_opt = this->adc->GetAdcVoltage(FORCE_SENSOR_CHANNEL);
        if (forceValue_opt.has_value()) {
            std::ostringstream ss;
            ss << std::chrono::duration_cast<std::chrono::milliseconds>(start - boot).count() << ';' << forceValue_opt.value();
            csv.WriteLine(ss.str(), false);
            if (i == 10) {
                j++;
                if (j == 10) {
                    ara::log::LogInfo() << "New tenso value: " << ss.str();
                    j = 0;
                }
                this->service_udp.newForceEvent.Update(forceValue_opt.value());
                i = 0;
            }
        } else {
            continue;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        core::condition::wait_for(std::chrono::milliseconds(LOOP_TIME_MS - duration.count()), token); 
    }
    csv.Close();
    service_ipc.StopOffer();
    service_udp.StopOffer();
    return core::ErrorCode::kOk;
}

int MyGhApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView> parms) {
    myPath = parms.at("app_path");

    // Initialize pressure sensor
    adc = std::make_unique<i2c::ADS7828>();
    auto i2c = std::make_unique<i2c::I2CController>();
    i2c->Init(std::make_unique<com::soc::StreamIpcSocket>());
    auto adc_init_res = adc->Init(std::move(i2c));
    if (adc_init_res != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize ADC";
        return core::ErrorCode::kInitializeError;
    }

    service_ipc.StartOffer();
    service_udp.StartOffer();
    ara::log::LogDebug() << "Initialize Complete";
    return core::ErrorCode::kOk;
}
}
}
