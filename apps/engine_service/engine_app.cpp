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

#include "apps/engine_service/engine_app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace simba {
namespace apps {

namespace {
  constexpr auto kPrimer_path_name = "simba/apps/PrimerService/PrimService_ipc";
  constexpr auto kServo_path_name = "simba/apps/servoService/ServoService_ipc";
  constexpr auto kEngine_path_name = "simba/apps/EngineService/EngineService_ipc";
  constexpr auto kEngine_udp_path_name = "simba/apps/EngineService/EngineService_udp";
}

EngineApp::EngineApp():
      primer_proxy(ara::core::InstanceSpecifier{kPrimer_path_name}),
      servo_proxy((ara::core::InstanceSpecifier{kServo_path_name})),
      service_ipc(ara::core::InstanceSpecifier{kEngine_path_name}, primer_handler_, servo_handler_, mode),
      service_udp(ara::core::InstanceSpecifier{kEngine_udp_path_name}, primer_handler_, servo_handler_, mode),
      primer_handler_{nullptr}, servo_handler_{nullptr}, mode(std::make_shared<service::MODE_t>()) {
  *mode = service::MODE_t::AUTO;
  service_ipc.StartOffer();
  service_udp.StartOffer();
}

int EngineApp::Run(const std::stop_token& token) {
  core::condition::wait(token);
  ara::log::LogInfo() << "Run complete, closing";
  // Closing app
  service_ipc.StopOffer();
  service_udp.StopOffer();
  servo_proxy.StopFindService();
  primer_proxy.StopFindService();
}

int EngineApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  servo_proxy.StartFindService([this](auto handler) {
    this->servo_handler_ = handler;
  });
  primer_proxy.StartFindService([this](auto handler) {
    this->primer_handler_ = handler;
  });
  ara::log::LogInfo() << "Initialize Complete";
}

}  // namespace apps
}  // namespace simba
