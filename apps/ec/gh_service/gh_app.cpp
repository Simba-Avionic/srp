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
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include <sys/resource.h>
#include "mw/i2c_service/controller/bme280/controller.hpp"

namespace srp {
    namespace apps {

        GhApp::GhApp(){
        }

        int GhApp::Run(const std::stop_token& token) {
            ara::log::LogInfo() << "Running GH Application";
            // Here we want to test our BME280 Driver
            
            while(1){
                auto pressure = bme->getPressure();
                auto temp = bme->getTemperature();
                auto hum = bme->getHumidity();
                ara::log::logInfo() << "Pressure: " << pressure << ", temperature: " << temperature << ", humidity: " << humidity;
                //wait
                core::condition::wait_for(std::chrono::milliseconds(3000), token);
            }

            core::condition::wait(token);
            ara::log::LogInfo() << "Run complete, closing";
            return 0;
        }

        int GhApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView> parms) {
            bme = std::make_unique<i2c::BME280>();
            auto i2c = std::make_unique<i2c::I2CController>();
            i2c->Init(std::make_unique<com::soc::StreamIpcSocket>())
            if(bme->Init(std::move(i2c)) != core::ErrorCode::kOk){
                ara::log::LogError() << "GhApp: Failed to initialize BME280 sensor";
            }


            myPath = parms.at("app_path");
            ara::log::LogInfo() << "Initialize Complete";
            return 0;
        }

    }
}
