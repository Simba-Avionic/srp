/**
 * @file apogee_app.hpp
 * @author Wiktor Laska
 * @version 0.1
 * @date 2026-04-25
 * @copyright Copyright (c) 2026
 */
#ifndef APPS_FC_APOGEE_SERVICE_HPP_
#define APPS_FC_APOGEE_SERVICE_HPP_

#include <atomic>
#include <chrono>
#include <memory>
#include <map>

// tak było w radio_app zapytać Mati- po co skąd itp
#include "ara/exec/adaptive_application.h"
#include "srp/env/EnvAppFc/EnvAppFcHandler.h"
#include "srp/nav/NavAppFc/NavAppFcHandler.h"
#include "srp/apps/ApogeeApp/ApogeeAppSkeleton.h"

#include "core/apogee/ApogeeDetector.h"
constexpr double kBasePressure = 1013.25; // hPa
namespace srp
{
namespace apps {
        class ApogeeService : public ara::exec::AdaptiveApplication
        {
        private:
            // zapytać co tam dokładnie czym jest
            env::EnvAppFcProxy env_service_proxy;
            std::shared_ptr<env::EnvAppFcHandler> env_service_handler;

            nav::NavAppFcProxy nav_service_proxy;
            std::shared_ptr<nav::NavAppFcHandler> nav_service_handler;

            std::unique_ptr<apps::ApogeeAppSkeleton> apogee_skeleton;

            RealTimeApogee apogee_detector_{15, -0.5, 0.0}; 

            std::atomic<double> latest_height_{0.0};
            std::atomic<double> latest_velocity_{0.0};

            std::chrono::steady_clock::time_point last_imu_time_;
            bool first_imu_measurement_ = true;


            const double base_pressure_ = kBasePressure; 
            
            void SomeIpInit();
            void EvaluateApogee();

        protected:
            int Run(const std::stop_token &token) override;
            int Initialize(const std::map<ara::core::StringView, ara::core::StringView> parms) override;

        public:
            ApogeeService();
            ~ApogeeService() = default;
        };
} // namespace apps
} // namespace srp

#endif // APPS_FC_APOGEE_SERVICE_HPP_