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
namespace srp {
namespace service {
namespace {
  constexpr auto kService_ipc_name = "srp/apps/RecoveryService/RecoveryService_ipc";
  constexpr auto kService_udp_name = "srp/apps/RecoveryService/RecoveryService_udp";
  constexpr auto KRec_did_name = "/srp/apps/RecoveryService/REC_RID";
  constexpr auto kRefresh_delay_time = 50;
}
RecoveryService::RecoveryService(): rec_did_specifier(KRec_did_name), rocket_started(false) {
  timestamp_.Init();
}
bool RecoveryService::DetectTargetHeight() {
  static auto activation_height = controller->GetTargetActivationHeight();
  static auto activation_time = controller->GetTargetActivationTime();
  auto actual_height = 500; // TODO() add read actual height
  auto now = timestamp_.GetNewTimeStamp();
  if (now.has_value()) {
    auto buckup_detected = timestamp_.GetDeltaTime(now.value(), apogee_time) >= activation_time;
    return (actual_height <= actual_height) || buckup_detected;
  }
  return actual_height <= actual_height;
}
bool RecoveryService::ApogeeDetected() {
  // TODO() make algorithm that detect apogee
  return false;
}

int RecoveryService::Run(const std::stop_token& token) {
  service_ipc->StartOffer();
  service_udp->StartOffer();
  while (!token.stop_requested()) {
    while (!rocket_started) {
      core::condition::wait_for(std::chrono::milliseconds(kRefresh_delay_time), token);
    }
    while (!ApogeeDetected()) {
      core::condition::wait_for(std::chrono::milliseconds(kRefresh_delay_time), token);
    }
    auto now = timestamp_.GetNewTimeStamp();
    if (now.has_value()) {
      apogee_time = now.value();
    } else {
      ara::log::LogError() << "cant set apogee time, backup unreef dont work properly";
    }
    this->controller->OpenParachute();
    while (!DetectTargetHeight()) {
      core::condition::wait_for(std::chrono::milliseconds(kRefresh_delay_time), token);
    }
    this->controller->UnreefParachute();
  }
  core::condition::wait(token);
  service_ipc->StopOffer();
  service_udp->StopOffer();
  rec_did->StopOffer();
  return 0;
}

int RecoveryService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  this->controller = std::make_shared<apps::recovery::ParachuteController>();
  auto pca = std::make_unique<i2c::PCA9685>();
  pca->Init("/srp/opt/RecoveryService/");
  // TODO(matikrajek42@gmail.com) change to parms.at("app_name") after BS fix adaptiveAplication
  this->controller->Init(std::move(pca), std::make_unique<gpio::GPIOController>(), "/srp/opt/RecoveryService/");
  rec_did = std::make_unique<apps::RecoveryGenericRoutine>(rec_did_specifier, controller);
  service_ipc = std::make_unique<apps::MyRecoveryServiceSkeleton>(
                ara::core::InstanceSpecifier(kService_ipc_name), controller);
  service_udp = std::make_unique<apps::MyRecoveryServiceSkeleton>(
                ara::core::InstanceSpecifier(kService_udp_name), controller);
  rec_did->Offer();
  return 0;
}

}  // namespace service
}  // namespace srp
