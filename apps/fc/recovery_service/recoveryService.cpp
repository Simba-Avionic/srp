/**
 * @file servoService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "apps/fc/recovery_service/recoveryService.hpp"

#include <memory>
#include <optional>
#include <vector>
#include <utility>
#include "ara/log/log.h"
#include "core/common/condition.h"
#include "core/common/error_code.h"
#include "apps/ec/ServoService/servoController/servo_controller.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
namespace srp {
namespace service {
namespace {
  constexpr auto kService_ipc_name = "srp/apps/RecoveryService/RecoveryService_ipc";
  constexpr auto kService_udp_name = "srp/apps/RecoveryService/RecoveryService_udp";
  constexpr auto KRec_did_name = "/srp/apps/RecoveryService/REC_RID";
    static constexpr auto kHeartBeatPinID = 4;
}
RecoveryService::RecoveryService(): rec_did_specifier(KRec_did_name) {
}


int RecoveryService::Run(const std::stop_token& token) {
  rec_did->Offer();
  service_ipc->StartOffer();
  service_udp->StartOffer();
  while (!token.stop_requested()) {
    if (gpio_.SetPinValue(kHeartBeatPinID, 1, 500) != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "EngineApp::Run: Failed to toggle heartbeat pin";
    }
    auto parachute_state = static_cast<uint8_t>(controller->GetParachuteState());
    service_ipc->NewParachuteStatusEvent.Update(parachute_state);
    service_udp->NewParachuteStatusEvent.Update(parachute_state);
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  service_ipc->StopOffer();
  service_udp->StopOffer();
  rec_did->StopOffer();
  return 0;
}

int RecoveryService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  this->controller = srp::apps::recovery::ParachuteController::GetInstance();
  const auto app_path = parms.at("app_path");
  this->controller->Init(app_path);
  rec_did = std::make_unique<apps::RecoveryGenericRoutine>(rec_did_specifier);
  service_ipc = std::make_unique<apps::MyRecoveryServiceSkeleton>(
                ara::core::InstanceSpecifier(kService_ipc_name));
  service_udp = std::make_unique<apps::MyRecoveryServiceSkeleton>(
                ara::core::InstanceSpecifier(kService_udp_name));
  return 0;
}

}  // namespace service
}  // namespace srp
