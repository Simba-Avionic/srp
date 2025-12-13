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
  constexpr auto kRefresh_delay_time = 50;
}
RecoveryService::RecoveryService(): rec_did_specifier(KRec_did_name) {
}


int RecoveryService::Run(const std::stop_token& token) {
  service_ipc->StartOffer();
  service_udp->StartOffer();
  while (!token.stop_requested()) {
    service_ipc->NewParachuteStatusEvent.Update(static_cast<uint8_t>(apps::recovery::ParachuteState_t::CLOSED));
    service_udp->NewParachuteStatusEvent.Update(static_cast<uint8_t>(apps::recovery::ParachuteState_t::CLOSED));
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  service_ipc->StopOffer();
  service_udp->StopOffer();
  rec_did->StopOffer();
  return 0;
}

int RecoveryService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  this->controller = std::make_shared<apps::recovery::ParachuteController>();
  const auto app_path = parms.at("app_path");
  auto servo_controller = std::make_shared<srp::service::ServoController>();
  auto init_res = servo_controller->Init(app_path);
  if (init_res != core::ErrorCode::kOk) {
    ara::log::LogError() << "RecoveryService.Initialize: unable to init servo controller";
    return 1;
  }
  this->controller->Init(std::move(servo_controller),
                         std::make_unique<gpio::GPIOController>(), app_path);
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
