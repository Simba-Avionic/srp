/**
 * @file apogee_app.cpp
 * @author Wiktor Laska
 * @version 0.1
 * @date 2026-04-25
 * @copyright Copyright (c) 2026
 */
#include "apogee_app.hpp"

#include <cmath>

#include "ara/log/log.h"
#include "core/common/condition.h"

namespace srp {
namespace apps {

namespace {
  static constexpr auto kEnv_service_path = "srp/apps/ApogeeDetectApp/EnvApp";
  static constexpr auto kService_ipc_instance = "srp/apps/ApogeeDetectApp/ApogeeDetectService_ipc";
  static constexpr auto kService_udp_instance = "srp/apps/ApogeeDetectApp/ApogeeDetectService_udp";
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
            this->EvaluateApogee();
        }
      });
    });
  });
  // TODO(wlaska): odczytywanie velocity
}

void ApogeeService::EvaluateApogee() {
    if (apogee_detector_.isApogeeReached()) {
        return;
    }
    double current_height = this->latest_height_.load();
    double current_velocity = this->latest_velocity_.load();

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

  while (!token.stop_requested()) {
    this->service_ipc.newApogeeDetected.Update(is_apogee_detected);
    this->service_udp.newApogeeDetected.Update(is_apogee_detected);
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  this->service_ipc.StopOffer();
  this->service_udp.StopOffer();
  ara::log::LogInfo() << "ApogeeService: Stopped.";

  return 0;
}

}  // namespace apps
}  // namespace srp
