/**
 * @file primer_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>
#include <vector>
#include <utility>
#include <future>  // NOLINT

#include "apps/ec/primer_service/app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace srp {
namespace primer {

namespace {
  constexpr auto kdid_path = "/srp/apps/PrimerService/primer_did";
}

int PrimerService::Run(const std::stop_token& token) {
  core::condition::wait(token);
  service_ipc.StopOffer();
  service_udp.StopOffer();
  primer_did_->StopOffer();
  return 0;
}

PrimerService::PrimerService() :
  controller(std::make_shared<PrimerController>()),
  service_ipc{ara::core::InstanceSpecifier{"srp/apps/PrimerService/PrimService_ipc"}, this->controller},
  service_udp{ara::core::InstanceSpecifier{"srp/apps/PrimerService/PrimService_udp"}, this->controller},
  did_instance{kdid_path} {
    primer_did_ = std::make_unique<PrimerDID>(did_instance, controller);
  }

  int PrimerService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  controller->Initialize(parms.at("app_path") + "etc/config.json");
  primer_did_->Offer();
  service_ipc.StartOffer();
  service_udp.StartOffer();
  return 0;
}

}  // namespace primer
}  // namespace srp
