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
    constexpr auto kCutdown_end_time_ms = 9500;
    constexpr auto kLoss_conn_time_to_drop_fuel = 10000;
    constexpr auto kPrimer_activation_delay_ms = 100;
}
RocketController::RocketController() {
    rocket_state_ = apps::RocketState::GetInstance();
    last_state_ = rocket_state_->GetState();
    timestamp_.Init();
}
void RocketController::Init() {
}
void RocketController::Loop() {
  auto now_state = rocket_state_->GetState();
  switch (now_state) {
    case RocketState_t::INIT:
    if (this->InitializeCompleted()) {
        rocket_state_->SetState(RocketState_t::DISARM);
    }
    break;
    case RocketState_t::DISARM:
    break;
    case RocketState_t::ARM:
    if (last_state_ != RocketState_t::ARM) {
        ArmRocket();
    }
    break;
    case RocketState_t::LAUNCH:
    this->CutdownEndSeq();
    this->rocket_state_->SetState(RocketState_t::FLIGHT);
    break;
    case RocketState_t::FLIGHT:
    // TODO(simba) IDK thath we need something
    break;
    case RocketState_t::MAIN_PARACHUTE_TRIGGERED:
    if (last_state_ != RocketState_t::MAIN_PARACHUTE_TRIGGERED) {
        // TODO(simba) call Ref Parachute open
        break;
    }

    break;
    case RocketState_t::SECOND_PARACHUTE_TRIGGERED:
    if (last_state_ != RocketState_t::SECOND_PARACHUTE_TRIGGERED) {
        // TODO(simba) call Main Parachute open
        // TODO(simba) open all actuator and disable power on actuator
    }
    break;
    case RocketState_t::ABORT:
        // TODO(simba) open vent valve
        // TODO(simba) disable all actuator power
    break;
    }
    last_state_ = now_state;
}

core::ErrorCode RocketController::InitializeCompleted() {
    return core::ErrorCode::kOk;
}
core::ErrorCode RocketController::ArmRocket() {
    // enable power on all stages
    return core::ErrorCode::kOk;
}
core::ErrorCode RocketController::CutdownEndSeq() {
    // open main valve
    std::this_thread::sleep_for(std::chrono::milliseconds(kPrimer_activation_delay_ms));
    // activate primer
    return core::ErrorCode::kOk;
}
core::ErrorCode RocketController::LossConnSeq() {
    return core::ErrorCode::kOk;
}

}  // namespace apps
}  // namespace srp
