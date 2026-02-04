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

namespace {
    constexpr uint8_t FORCE_SENSOR_CHANNEL = 3;
    constexpr auto LOOP_TIME_MS = 2000000;
}


namespace srp {
    namespace apps {

        MyGhApp::MyGhApp(): service_ipc{ara::core::InstanceSpecifier{"srp/apps/GhApp/ghService_ipc"}},
                        service_udp{ara::core::InstanceSpecifier{"srp/apps/GhApp/ghService_udp"}} {
        }


        int MyGhApp::Run(const std::stop_token& token) {
            ara::log::LogInfo() << "Running GH Application";
            uint32_t iter = 0;
            while (!token.stop_requested()) {
                // auto start = std::chrono::high_resolution_clock::now();
                auto forceValue_opt = this->adc->GetAdcVoltage(FORCE_SENSOR_CHANNEL);
                if (forceValue_opt.has_value()) {
                    auto forceValue = forceValue_opt.value();
                    // std::ostringstream ss;
                    // ss << std::fixed << std::setprecision(2) << forceValue;
                    // ara::log::LogDebug() << "Receive new force: " << ss.str() << "";
                    // this->service_ipc.newForceEvent.Update(static_cast<uint16_t>(forceValue * 100));
                    this->service_udp.newForceEvent.Update(static_cast<uint16_t>(forceValue * 100));
                    if((iter++)%100 == 0){
                        ara::log::LogWarn() << std::to_string(iter);
                    }
                } else {
                    // ara::log::LogWarn() << "Didn't receive new force";
                }
                // auto end = std::chrono::high_resolution_clock::now();
                // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                // if(iter == 100){
                //     ara::log::LogWarn() << "Loop has taken:" << std::to_string(duration.count()) << "ns";
                //     iter = 0;
                // }
                // core::condition::wait_for(std::chrono::nanoseconds(LOOP_TIME_MS - duration.count()), token); 
            }
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
