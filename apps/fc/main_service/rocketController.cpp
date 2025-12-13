/**
 * @file rocketController.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "apps/fc/main_service/rocketController.hpp"
#include "core/common/condition.h"
namespace srp {
namespace apps {
namespace {
}
RocketController::RocketController() {
    rocket_state_ = apps::RocketState::GetInstance();
    last_state_ = rocket_state_->GetState();
    timestamp_.Init();
    Loop(last_state_);
}
void RocketController::Init() {
}

void RocketController::SecureLoop() {
    auto now_state = rocket_state_->GetState();
    if (now_state == last_state_) {
        return;
    }
    Loop(now_state);
}

void RocketController::Loop(const RocketState_t new_state) {
  switch (new_state) {
    case RocketState_t::INIT:
        rocket_state_->SetState(RocketState_t::DISARM);
        // TODO(matikrajek42@gmail.com) Maybe we need to do something here?
        break;
    case RocketState_t::DISARM:
        break;
    case RocketState_t::ARM:
        // TODO(matikrajek42@gmail.com) Here add activation of Power Switch
        break;
    case RocketState_t::LAUNCH:

        rocket_state_->SetState(RocketState_t::FLIGHT);
        break;
    case RocketState_t::FLIGHT:
        // TODO(matikrajek42@gmail.com) Here we need to search for apogee
        break;
    case RocketState_t::MAIN_PARACHUTE_TRIGGERED:
        // TODO(matikrajek42@gmail.com) Activate Parachute and wait for linecutter
        break;
    case RocketState_t::SECOND_PARACHUTE_TRIGGERED:
        // TODO(matikrajek42@gmail.com) Activate LineCutter and stay there
        // TODO(matikrajek42@gmail.com) Open Main Valve, Vent Valve, Dump Valve, disactivate all actuators
        break;
    case RocketState_t::ABORT:
        break;
    }
    last_state_ = new_state;
}

}  // namespace apps
}  // namespace srp
