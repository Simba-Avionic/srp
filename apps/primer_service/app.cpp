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

#include "apps/primer_service/app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace simba {
namespace primer {

int PrimerService::Run(const std::stop_token& token) {
  core::condition::wait(token);
  service_ipc.StopOffer();
  service_udp.StopOffer();
  return 0;
}

PrimerService::PrimerService() :
  controller(std::make_shared<PrimerController>()),
  service_ipc{ara::core::InstanceSpecifier{"simba/apps/PrimerService/PrimService_ipc"}, this->controller},
  service_udp{ara::core::InstanceSpecifier{"simba/apps/PrimerService/PrimService_udp"}, this->controller} {}

  int PrimerService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  controller->Initialize(parms.at("app_path") + "etc/config.json");
  service_ipc.StartOffer();
  service_udp.StartOffer();
  return 0;
}

}  // namespace primer
}  // namespace simba
