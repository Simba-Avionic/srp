/**
 * @file engine_reactions_Rurku.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Reakcje EngineApp na zmiany stanu - Rurku (na razie kopia R8); reszta aplikacji w engine_app.cpp
 * @version 0.1
 * @date 2026-11-22
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <chrono>  // NOLINT
#include <thread>  // NOLINT

#include "apps/ec/engine_service/engine_app.hpp"
#include "ara/log/log.h"

namespace srp {
namespace apps {

void EngineApp::OnArm() {
  for (const auto & pin : arm_pins_id) {
    if (gpio_.SetPinValue(pin.pin_id, kPin_on) != core::ErrorCode::kOk) {
      ara::log::LogError() << "cant arm pin: " << pin.name;
    }
  }
}

void EngineApp::OnDisarm() {
  ara::log::LogInfo() << "EngineApp::OnDisarm: DISARM sequence completed";
  for (const ArmPinConfig_t& pin : arm_pins_id) {
    if (gpio_.SetPinValue(pin.pin_id, kPin_off, 0)) {
      ara::log::LogError() << "cant disarm pin: " << pin.name;
    }
  }
  servo_handler_->SetOxidizerVentValve(0);
  servo_handler_->SetOxidizerDumpValve(0);
}

void EngineApp::OnLaunch() {
  ara::log::LogInfo() << "EngineApp::OnLaunch: Launch initiated - starting async launch sequence";
  std::thread([this]() {
    auto res = this->primer_handler_->StartPrime();
    // if (!res.HasValue()) {
    //   ara::log::LogError() << "Invalid request to MW:GPIOService";
    //   return;
    // }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    servo_handler_->SetPressureFeedVentValve(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(450));
    servo_handler_->SetOxidizerMainValve(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(7000));
    // end of the launch sequence: LAUNCH -> FLIGHT; the valves stay open in FLIGHT (closing them is a future state);
    // ABORT is rejected from LAUNCH (F-02)
    if (!state_ctr->SetState(core::rocketState::RocketState_t::FLIGHT)) {
      // e.g. an external SetMode(FLIGHT) during the first 1.45 s already moved the state: the valves opened later
      // by this thread would stay open
      ara::log::LogError() << "EngineApp::OnLaunch: FLIGHT rejected at the end of the launch sequence, "
                              "closing valves directly";
      this->CloseValves();
    }
  }).detach();
}

void EngineApp::OnApogee() {
  ara::log::LogInfo() << "EngineApp::OnApogee: Apogee detected - triggering abort sequence";
  for (const ArmPinConfig_t& pin : arm_pins_id) {
    bool disable_later = (pin.func == "VS" || pin.func == "PFVS" || pin.func == "MS");
    if (gpio_.SetPinValue(pin.pin_id,
                          disable_later ? kPin_on : kPin_off,
                          disable_later ? 3500 : 0,
                          disable_later) != core::ErrorCode::kOk) {
      ara::log::LogError() << "cant disarm pin: " << pin.name;
    }
  }
  if (servo_handler_ != nullptr) {
    servo_handler_->SetOxidizerDumpValve(1);
    servo_handler_->SetOxidizerVentValve(1);
  }
}

void EngineApp::OnAbort() {
  ara::log::LogWarn() << "EngineApp::OnAbort: ABORT sequence initiated";

  // F-03: the EC valves are closed even when the sec_ec servo service is missing (it is not used here at all)
  if (sec_servo_handler_ == nullptr) {
    ara::log::LogError() << "EngineApp::OnAbort: SecServoService handler is nullptr (no sec_ec?), "
                            "executing the EC part of the ABORT only";
  }
  if (servo_handler_ == nullptr) {
    ara::log::LogError() << "EngineApp::OnAbort: ServoService handler is nullptr, ABORT valves not closed";
    return;
  }
  std::thread([this]() {
    this->CloseValves();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    servo_handler_->SetOxidizerMainValve(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    servo_handler_->SetPressureFeedVentValve(1);
  }).detach();
}

void EngineApp::OnConnectionLost() {
  this->OnDisarm();
}

// stany bez reakcji EC: puste callbacki
void EngineApp::OnInit() {}

void EngineApp::OnFlight() {}

void EngineApp::OnFirstParachute() {}

void EngineApp::OnSecondParachute() {}

void EngineApp::OnDrop() {}

void EngineApp::OnTouchdown() {}

void EngineApp::CloseValves() {
  servo_handler_->SetOxidizerMainValve(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  servo_handler_->SetPressureFeedVentValve(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  servo_handler_->SetPressureFeedMainValve(0);
}

}  // namespace apps
}  // namespace srp
