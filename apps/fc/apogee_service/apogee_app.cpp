/**
 * @file apogee_app.cpp
 * @author Wiktor Laska
 * @version 0.1
 * @date 2026-04-25
 * @copyright Copyright (c) 2026
 */
#include "apogee_app.hpp"
#include "ara/log/log.h"
#include "core/common/condition.h"
#include <cmath>

namespace srp {
namespace apogee {

namespace {
  static constexpr auto kEnv_service_path = "srp/env/EnvAppFc/envServiceFc_ipc";
  static constexpr auto kNav_service_path = "srp/nav/NavAppFc/navServiceFc_ipc";
  static constexpr auto kApogee_service_path = "srp/apps/ApogeeApp/ApogeeService";
}
// czym jest ara i jak działa
ApogeeService::ApogeeService() : 
    env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path}},
    nav_service_proxy{ara::core::InstanceSpecifier{kNav_service_path}},
    env_service_handler{nullptr},
    nav_service_handler{nullptr} {
    
    apogee_skeleton = std::make_unique<apps::ApogeeAppSkeleton>(
        ara::core::InstanceSpecifier{kApogee_service_path});
}

int ApogeeService::Initialize(const std::map<ara::core::StringView, ara::core::StringView> parms) {
    ara::log::LogInfo() << "ApogeeService: Initializing...";
    
    apogee_skeleton->StartOffer(); 
    
    this->SomeIpInit();
    
    return 0;
}

void ApogeeService::SomeIpInit() {
  
  
  this->env_service_proxy.StartFindService([this](auto handler) {
    this->env_service_handler = handler;
    //przekopiowane z radio_app do subskrypcji
    //przeliczanie dancych aby pasowały do detektora apogeum
    this->env_service_handler->newBME280Event.Subscribe(1, [this](const uint8_t status) {
      this->env_service_handler->newBME280Event.SetReceiveHandler([this] () {
        
        auto res = this->env_service_handler->newBME280Event.GetNewSamples();
        if (res.HasValue()) {
            double current_pressure = res.Value().pressure;
            double height = 44330.0 * (1.0 - std::pow(current_pressure / base_pressure_, 0.1903));
            
            this->latest_height_.store(height);
            this->EvaluateApogee();
        }
      });
    });
  });

  this->nav_service_proxy.StartFindService([this](auto handler) {
    this->nav_service_handler = handler;
    
    this->nav_service_handler->newImuEvent.Subscribe(1, [this](const uint8_t status) {
      this->nav_service_handler->newImuEvent.SetReceiveHandler([this] () {
        auto res = this->nav_service_handler->newImuEvent.GetNewSamples();
        if (res.HasValue()) {
            double accel_z = res.Value().acceleration_z; 
            
            auto now = std::chrono::steady_clock::now();
            
            if (first_imu_measurement_) {
                last_imu_time_ = now;
                first_imu_measurement_ = false;
                return;
            }
            std::chrono::duration<double> dt_seconds = now - last_imu_time_;
            double dt = dt_seconds.count();
            last_imu_time_ = now;

            double old_velocity = this->latest_velocity_.load();
            double new_velocity = old_velocity + ((accel_z - 9.81) * dt);
            
            this->latest_velocity_.store(new_velocity);
            this->EvaluateApogee();
        }
      });
    });
  });
}

void ApogeeService::EvaluateApogee() {
    if (apogee_detector_.isApogeeReached()) {
        return; 
    }

    double current_height = this->latest_height_.load();
    double current_velocity = this->latest_velocity_.load();

    apogee_detector_.update(current_height, current_velocity);

    if (apogee_detector_.isApogeeReached()) {
        ara::log::LogInfo() << "APOGEUM WYKRYT: "<< "Max Wysokosc: " << apogee_detector_.getApogee() << " m, "<< "Ostatnia Predkosc: " << apogee_detector_.averageSpeed() << " m/s";
        // jak rozsyłać ???
    }
}

int ApogeeService::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "ApogeeService: Running in event-driven mode.";
  
  while (!token.stop_requested()) {
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  apogee_skeleton->StopOffer();
  ara::log::LogInfo() << "ApogeeService: Stopped.";
  
  return 0;
}

} // namespace apogee
} // namespace srp