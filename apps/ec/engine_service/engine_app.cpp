/**
 * @file engine_app.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "apps/ec/engine_service/engine_app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace simba {
namespace apps {

namespace {
  constexpr auto kPrimer_path_name = "simba/apps/EngineService/PrimerService";
  constexpr auto kServo_path_name = "simba/apps/EngineService/ServoService";
  constexpr auto kEngine_path_name = "simba/apps/EngineService/EngineService_ipc";
  constexpr auto kEngine_udp_path_name = "simba/apps/EngineService/EngineService_udp";
}

EngineApp::EngineApp():
      primer_proxy(ara::core::InstanceSpecifier{kPrimer_path_name}),
      servo_proxy((ara::core::InstanceSpecifier{kServo_path_name})),
      service_ipc(ara::core::InstanceSpecifier{kEngine_path_name}),
      service_udp(ara::core::InstanceSpecifier{kEngine_udp_path_name}),
      primer_handler_{nullptr}, servo_handler_{nullptr} {
}

int EngineApp::Run(const std::stop_token& token) {
  core::condition::wait(token);
  ara::log::LogInfo() << "Run complete, closing";
  service_ipc.StopOffer();
  service_udp.StopOffer();
  servo_proxy.StopFindService();
  primer_proxy.StopFindService();
  return 0;
}

int EngineApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  servo_proxy.StartFindService([this](auto handler) {
    servo_handler_ = handler;
  });
  primer_proxy.StartFindService([this](auto handler) {
    this->primer_handler_ = handler;
  });

  this->service_ipc.Init(primer_handler_, servo_handler_);
  this->service_udp.Init(primer_handler_, servo_handler_);
  service_ipc.StartOffer();
  service_udp.StartOffer();
  ara::log::LogInfo() << "Initialize Complete";
  return 0;
}

}  // namespace apps
}  // namespace simba
