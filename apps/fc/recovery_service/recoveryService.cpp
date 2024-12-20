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
namespace simba {
namespace service {
namespace {
  constexpr auto kService_ipc_name = "simba/apps/RecoveryService/RecoveryService_ipc";
  constexpr auto kService_udp_name = "simba/apps/RecoveryService/RecoveryService_udp";
  constexpr auto KRec_did_name = "/simba/apps/RecoveryService/REC_RID";
}
RecoveryService::RecoveryService(): rec_did_specifier(KRec_did_name) {
}

int RecoveryService::Run(const std::stop_token& token) {
  service_ipc->StartOffer();
  service_udp->StartOffer();
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
  this->controller->Init(std::move(pca), std::make_unique<gpio::GPIOController>());
  rec_did = std::make_unique<apps::RecoveryGenericRoutine>(rec_did_specifier, controller);
  service_ipc = std::make_unique<apps::MyRecoveryServiceSkeleton>(
                ara::core::InstanceSpecifier(kService_ipc_name), controller);
  service_udp = std::make_unique<apps::MyRecoveryServiceSkeleton>(
                ara::core::InstanceSpecifier(kService_udp_name), controller);
  rec_did->Offer();
  return 0;
}

}  // namespace service
}  // namespace simba
