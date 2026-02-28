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
  constexpr auto kPrimerDelay = 1000;
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
  service_ipc.StopOffer();
  service_udp.StopOffer();
  servo_proxy.StopFindService();
  primer_proxy.StopFindService();
  return 0;
}

int EngineApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  state_ctr = core::rocketState::RocketStateController::GetInstance();
  state_ctr->RegisterOnStateChangeCallback([this](core::rocketState::RocketState_t state) {
        this->OnStateChange(state);
  });
  state_ctr->RegisterCallback(core::rocketState::RocketState_t::LAUNCH, [this]() {
      this->OnLaunch();
  });
  state_ctr->RegisterCallback(core::rocketState::RocketState_t::ARM, [this]() {
      this->OnArm();
  });
  state_ctr->RegisterCallback(core::rocketState::RocketState_t::DISARM, [this]() {
      this->OnDisarm();
  });
  state_ctr->RegisterCallback(core::rocketState::RocketState_t::APOGEE, [this]() {
      this->OnApogee();
  });


  service_ipc.StartOffer();
  service_udp.StartOffer();

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

  if (state_ctr == nullptr) {
    ara::log::LogError() << "EngineApp::Initialize: state_ctr_ is nullptr!";
    return 1;
  }
  ara::log::LogInfo() << "Initialize Complete";
  state_ctr->SetState(core::rocketState::RocketState_t::DISARM);
  return 0;
}

void EngineApp::OnStateChange(core::rocketState::RocketState_t new_state) {
  service_ipc.CurrentMode.Update(static_cast<uint8_t>(new_state));
  service_udp.CurrentMode.Update(static_cast<uint8_t>(new_state));
}

bool EngineApp::CheckLaunchRequirements() {
  if ((primer_handler_ == nullptr || servo_handler_ == nullptr)) {
      ara::log::LogError() << "Invalid pointer to Primer or Servo";
      return false;
  }
}

void EngineApp::OnLaunch() {
  if (!CheckLaunchRequirements()) {
    state_ctr->SetState(core::rocketState::RocketState_t::ABORT);
    return;
  }
  auto res = this->primer_handler_->OnPrime();
  if (!res.HasValue()) {
    ara::log::LogError() << "Invalid request to MW:GPIOService";
    return;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(kPrimerDelay));
  auto res2 = this->servo_handler_->SetMainServoValue(1);
  this->primer_handler_->OffPrime();
  if (!res2.HasValue()) {
    ara::log::LogError() << "Invalid request to MW:I2CService";
    return;
  }
  state_ctr->SetState(core::rocketState::RocketState_t::FLIGHT);
}

void EngineApp::OnArm() {
  // TODO(matikrajek42@gmail.com) Add ARM
}
void EngineApp::OnDisarm() {
  // TODO(matikrajek42@gmail.com) DISARM
}
void EngineApp::OnApogee() {
  // TODO(matikrajek42@gmail.com) Vent Valve & Dump Valve
}

}  // namespace apps
}  // namespace srp
