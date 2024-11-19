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
      //service_ipc(ara::core::InstanceSpecifier{kEngine_path_name}),
      //service_udp(ara::core::InstanceSpecifier{kEngine_udp_path_name}),
      primer_handler_{nullptr}, servo_handler_{nullptr} {
}

int EngineApp::Run(const std::stop_token& token) {
  servo_handler_->StartFind(token);
  primer_handler_->StartFind(token);
  //service_ipc.StartOffer();
  //service_udp.StartOffer();

  core::condition::wait(token);
  ara::log::LogInfo() << "Run complete, closing";
  // Closing app
  //service_ipc.StopOffer();
  //service_udp.StopOffer();
  servo_handler_->StopFind();
  primer_handler_->StopFind();
  servo_proxy.StopFindService();
  primer_proxy.StopFindService();
}

int EngineApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  ara::log::LogError() << "123";
  servo_proxy.StartFindService([this](auto handler) {
    ara::log::LogError() << "2";
    servo_handler_ = handler;
    ara::log::LogError() << "22";
  });
  ara::log::LogError() << "3";
  primer_proxy.StartFindService([this](auto handler) {
    this->primer_handler_ = handler;
    ara::log::LogError() << "4";
  });
  
  //this->service_ipc.Init(primer_handler_, servo_handler_);
  //this->service_udp.Init(primer_handler_, servo_handler_);
  ara::log::LogInfo() << "Initialize Complete";
}

}  // namespace apps
}  // namespace simba
