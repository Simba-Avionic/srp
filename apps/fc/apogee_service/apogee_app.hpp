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

#include "ara/exec/adaptive_application.h"
#include "srp/apps/ApogeeDetectServiceSkeleton.h"
#include "srp/env/EnvApp/EnvAppHandler.h"
#include "core/apogee/ApogeeDetector.h"

constexpr double kBasePressure = 1013.25;
constexpr double kEncodedBarToHpa = 10.0;

namespace srp
{
namespace apps {
        class ApogeeService : public ara::exec::AdaptiveApplication
        {
        private:
            // zapytać co tam dokładnie czym jest
            env::EnvAppProxy env_service_proxy;
            std::shared_ptr<env::EnvAppHandler> env_service_handler;
            apps::ApogeeDetectServiceSkeleton service_ipc;
            apps::ApogeeDetectServiceSkeleton service_udp;
            RealTimeApogee apogee_detector_{15, -0.5, 0.0}; 
            
            std::atomic<double> latest_height_{0.0};
            std::atomic<double> latest_velocity_{0.0};

            std::chrono::steady_clock::time_point last_imu_time_;
            bool first_imu_measurement_ = true;

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
