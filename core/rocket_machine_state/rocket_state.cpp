/**
 * @file rocket_state.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-02-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "core/rocket_machine_state/rocket_state.hpp"
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <string>
#include "ara/log/log.h"

namespace srp {
namespace core {
namespace rocketState {

namespace {
    static const std::unordered_map<RocketState_t, std::vector<RocketState_t>> allowed_transitions = {
        {RocketState_t::INIT,                     {RocketState_t::DISARM}},
        {RocketState_t::DISARM,                   {RocketState_t::ARM, RocketState_t::CONNECTION_LOST}},
        {RocketState_t::ARM,                      {RocketState_t::LAUNCH, RocketState_t::ABORT,
                                                    RocketState_t::DISARM, RocketState_t::CONNECTION_LOST}},
        {RocketState_t::ABORT,                    {RocketState_t::DISARM}},
        {RocketState_t::LAUNCH,                   {RocketState_t::FLIGHT}},
        {RocketState_t::FLIGHT,                   {RocketState_t::APOGEE}},
        {RocketState_t::APOGEE,                   {RocketState_t::FIRST_PARACHUTE}},
        {RocketState_t::FIRST_PARACHUTE,          {RocketState_t::SECOND_PARACHUTE}},
        {RocketState_t::SECOND_PARACHUTE,         {RocketState_t::DROP}},
        {RocketState_t::DROP,                     {RocketState_t::TOUCHDOWN}},
        {RocketState_t::CONNECTION_LOST,          {RocketState_t::DISARM, RocketState_t::ABORT}}
    };
    static std::shared_ptr<RocketStateController> instance = nullptr;
}  // namespace

void RocketStateController::RegisterCallback(RocketState_t state, StateCallback cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    callbacks_[state] = cb;
}

std::shared_ptr<RocketStateController> RocketStateController::GetInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<RocketStateController>();
    }
    return instance;
}

RocketStateController::RocketStateController(): actual_state{RocketState_t::INIT}, prev_state{RocketState_t::INIT} {
    RegisterOnStateChangeCallback([](RocketState_t new_state) {
    });
    RegisterRequirementsCallback([](RocketState_t new_state) {
        return true;
    });
}

bool RocketStateController::TransitionAllowed(const RocketState_t actual_state, const RocketState_t req_state) {
    auto it = allowed_transitions.find(actual_state);
    if (it == allowed_transitions.end()) {
        return false;
    }
    if (std::find(it->second.begin(), it->second.end(), req_state) == it->second.end()) {
        return false;
    }
    return true;
}

void RocketStateController::RegisterOnStateChangeCallback(OnStateChangeCallback cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    on_change_callback_ = cb;
}

RocketState_t RocketStateController::GetState() {
    std::lock_guard<std::mutex> lock_(mtx_);
    return actual_state;
}

void RocketStateController::RegisterRequirementsCallback(ChangeRequestCallback cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    requirements_callback_ = cb;
}

bool RocketStateController::SetState(const RocketState_t state) {
    std::lock_guard<std::mutex> lock_(mtx_);
    if (state == actual_state) {
        ara::log::LogInfo() << "Requested change to same state: " << core::rocketState::to_string(state);
        return false;
    }
    if (!TransitionAllowed(this->actual_state, state)) {
        ara::log::LogWarn() << "Transition from: " << core::rocketState::to_string(actual_state) <<
                                " To state: " << core::rocketState::to_string(state) << " rejected";
        return false;
    }
    if (!requirements_callback_(state)) {
        ara::log::LogWarn() << "Requirements were not met to change state";
        return false;
    }

    this->prev_state = actual_state;
    this->actual_state = state;

    if (callbacks_.count(state)) {
        callbacks_[state]();
    }

    on_change_callback_(actual_state);
    return true;
}

}  // namespace rocketState
}  // namespace core
}  // namespace srp
