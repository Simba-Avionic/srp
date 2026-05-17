/**
 * @file apogee_app.cpp
 * @author Wiktor Laska
 * @version 0.1
 * @date 2026-04-25
 * @copyright Copyright (c) 2026
 */
#include "apogee_app.hpp"

#include <cmath>
#include <thread>  // NOLINT

#include "ara/log/log.h"
#include "core/common/condition.h"

namespace srp {
namespace apps {

namespace {
  static constexpr auto kEnv_service_path = "srp/apps/ApogeeDetectApp/EnvApp";
  static constexpr auto kService_ipc_instance = "srp/apps/ApogeeDetectApp/ApogeeDetectService_ipc";
  static constexpr auto kService_udp_instance = "srp/apps/ApogeeDetectApp/ApogeeDetectService_udp";
  static constexpr auto kAlgorithm_delay_ms = 100;
  static constexpr auto kMain_parachute_opening_apogee_m = 400;
}

ApogeeService::ApogeeService() :
    env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path}},
    env_service_handler{nullptr},
    service_ipc{ara::core::InstanceSpecifier{kService_ipc_instance}},
    service_udp{ara::core::InstanceSpecifier{kService_udp_instance}} {
}

int ApogeeService::Initialize(const std::map<ara::core::StringView, ara::core::StringView> parms) {
    ara::log::LogInfo() << "ApogeeService: Initializing...";

    this->service_ipc.StartOffer();
    this->service_udp.StartOffer();
    this->SomeIpInit();

    return 0;
}

void ApogeeService::SomeIpInit() {
  this->env_service_proxy.StartFindService([this](auto handler) {
    this->env_service_handler = handler;
    this->env_service_handler->newBME280Event.Subscribe(1, [this](const uint8_t status) {
      this->env_service_handler->newBME280Event.SetReceiveHandler([this] () {
        auto res = this->env_service_handler->newBME280Event.GetNewSamples();
        if (res.HasValue()) {
            this->latest_height_.store(res.Value().altitude);
        }
      });
    });
    this->env_service_handler->newIMUEvent.Subscribe(1, [this](const uint8_t status) {
      this->env_service_handler->newIMUEvent.SetReceiveHandler([this]() {
        auto res = this->env_service_handler->newIMUEvent.GetNewSamples();
        if (res.HasValue()) {
          if (!first_imu_measurement_) {
            first_imu_measurement_ = true;
          }
          // TODO(matikrajek42@gmail.com) check wchich axis is valid
          this->latest_velocity_ = res.Value().accel_z;
        }
      });
    });
  });
}

void ApogeeService::EvaluateApogee() {
    if (!first_imu_measurement_) {
      return;
    }
    if (is_apogee_detected) {
      if (this->latest_height_ <= kMain_parachute_opening_apogee_m) {
        is_main_parachute_detected = true;
        this->service_ipc.newMainParachuteDetected.Update(true);
        this->service_udp.newMainParachuteDetected.Update(true);
      }
      return;
    }
    const double current_height = this->latest_height_.load();
    const double current_velocity = this->latest_velocity_.load();

    apogee_detector_.update(current_height, current_velocity);

    if (apogee_detector_.isApogeeReached()) {
      is_apogee_detected = true;
      ara::log::LogInfo() << "APOGEUM WYKRYT: " << "Max Wysokosc: "
          << static_cast<float>(apogee_detector_.getApogee()) << " m, "
          << "Ostatnia Predkosc: " << static_cast<float>(apogee_detector_.averageSpeed()) << " m/s";
      this->service_ipc.newApogeeDetected.Update(true);
      this->service_udp.newApogeeDetected.Update(true);
    }
}

int ApogeeService::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "ApogeeService: Running in event-driven mode.";

  auto apogeeThread = std::jthread([this](const std::stop_token& token){
    while(!token.stop_requested()) {
      EvaluateApogee();
      core::condition::wait_for(std::chrono::milliseconds(kAlgorithm_delay_ms), token);
    }
  });

  while (!token.stop_requested()) {
    this->service_ipc.newApogeeDetected.Update(is_apogee_detected);
    this->service_udp.newApogeeDetected.Update(is_apogee_detected);
    this->service_ipc.newMainParachuteDetected.Update(is_main_parachute_detected);
    this->service_udp.newMainParachuteDetected.Update(is_main_parachute_detected);
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  this->service_ipc.StopOffer();
  this->service_udp.StopOffer();
  ara::log::LogInfo() << "ApogeeService: Stopped.";

  return 0;
}

}  // namespace apps
}  // namespace srp
