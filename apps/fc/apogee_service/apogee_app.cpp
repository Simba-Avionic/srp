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
    this->env_service_handler->newPressEvent.Subscribe(1, [this](const uint8_t status) {
      this->env_service_handler->newPressEvent.SetReceiveHandler([this] () {
        auto res = this->env_service_handler->newPressEvent.GetNewSamples();
        if (res.HasValue()) {
            const double current_pressure = static_cast<double>(res.Value()) * kEncodedBarToHpa;
            const double height = 44330.0 * (1.0 - std::pow(current_pressure / kBasePressure, 0.1903));
            this->latest_height_.store(height);
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
        ara::log::LogInfo() << "APOGEUM WYKRYT: " << "Max Wysokosc: "
            << static_cast<float>(apogee_detector_.getApogee()) << " m, "
            << "Ostatnia Predkosc: " << static_cast<float>(apogee_detector_.averageSpeed()) << " m/s";
        this->service_ipc.ApogeeDetected.Update(true);
        this->service_udp.ApogeeDetected.Update(true);
    }
}

int ApogeeService::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "ApogeeService: Running in event-driven mode.";

  while (!token.stop_requested()) {
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  this->service_ipc.StopOffer();
  this->service_udp.StopOffer();
  ara::log::LogInfo() << "ApogeeService: Stopped.";

  return 0;
}

}  // namespace apps
}  // namespace srp
