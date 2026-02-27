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

namespace srp {
namespace apps {

namespace {
  constexpr auto kPrimer_path_name = "srp/apps/EngineService/PrimerService";
  constexpr auto kServo_path_name = "srp/apps/EngineService/ServoService";
  constexpr auto kEngine_path_name = "srp/apps/EngineService/EngineService_ipc";
  constexpr auto kEngine_udp_path_name = "srp/apps/EngineService/EngineService_udp";
  constexpr auto kInit_max_intervals = 20;
}

EngineApp::EngineApp():
      primer_proxy(ara::core::InstanceSpecifier{kPrimer_path_name}),
      servo_proxy((ara::core::InstanceSpecifier{kServo_path_name})),
      service_ipc(ara::core::InstanceSpecifier{kEngine_path_name}),
      service_udp(ara::core::InstanceSpecifier{kEngine_udp_path_name}),
      primer_handler_{nullptr}, servo_handler_{nullptr} {
}

int EngineApp::Run(const std::stop_token& token) {
  while (!token.stop_requested()) {
    service_ipc.CurrentMode.Update(static_cast<uint8_t>(state_ctr_->GetEngineState()));
    service_udp.CurrentMode.Update(static_cast<uint8_t>(state_ctr_->GetEngineState()));
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  ara::log::LogInfo() << "Run complete, closing";
  service_ipc.StopOffer();
  service_udp.StopOffer();
  servo_proxy.StopFindService();
  primer_proxy.StopFindService();
  return 0;
}

int EngineApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  state_ctr_ = engineApp::EngineStateController::GetInstance();
  state_ctr_->SetEngineState(apps::RocketState_t::DISARM);
  servo_proxy.StartFindService([this](auto handler) {
    servo_handler_ = handler;
  });
  primer_proxy.StartFindService([this](auto handler) {
    this->primer_handler_ = handler;
  });
  int i = 0;
  while ((servo_handler_ == nullptr || primer_handler_ == nullptr) && i < kInit_max_intervals) {
    i += 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  if (i >= kInit_max_intervals) {
    ara::log::LogFatal() << "Cant initialize servo handler or primer handler, closing app";
    return 1;
  }
  this->service_ipc.Init(primer_handler_, servo_handler_);
  this->service_udp.Init(primer_handler_, servo_handler_);

  service_ipc.StartOffer();
  service_udp.StartOffer();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  if (state_ctr_ == nullptr) {
    ara::log::LogError() << "EngineApp::Initialize: state_ctr_ is nullptr!";
    return 1;
  }
  ara::log::LogInfo() << "Initialize Complete";
  return 0;
}

}  // namespace apps
}  // namespace srp
