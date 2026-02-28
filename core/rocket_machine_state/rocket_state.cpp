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
#include <vector>
#include <string>
#include "ara/log/log.h"

namespace srp {
namespace core {
namespace rocketState {

namespace {
    static const std::unordered_map<RocketState_t, std::vector<RocketState_t>> allowed_transitions = {
        {RocketState_t::INIT,                     {RocketState_t::DISARM}},
        {RocketState_t::DISARM,                   {RocketState_t::ARM, RocketState_t::ABORT}},
        {RocketState_t::ARM,                      {RocketState_t::LAUNCH, RocketState_t::ABORT, RocketState_t::DISARM}},
        {RocketState_t::ABORT,                    {RocketState_t::DISARM}},
        {RocketState_t::LAUNCH,                   {RocketState_t::FLIGHT}},
        {RocketState_t::FLIGHT,                   {RocketState_t::APOGEE}},
        {RocketState_t::APOGEE, {RocketState_t::FIRST_PARACHUTE}},
        {RocketState_t::FIRST_PARACHUTE, {RocketState_t::SECOND_PARACHUTE}},
        {RocketState_t::SECOND_PARACHUTE, {RocketState_t::DROP}}
    };
    static std::shared_ptr<RocketStateController> instance = nullptr;

    std::string to_string(const RocketState_t state) {
        switch (state) {
            case RocketState_t::INIT:
                return "INIT";

            case RocketState_t::DISARM:
                return "DISARM";

            case RocketState_t::ARM:
                return "ARM";

            case RocketState_t::LAUNCH:
                return "LAUNCH";

            case RocketState_t::FLIGHT:
                return "FLIGHT";

            case RocketState_t::APOGEE:
                return "APOGEE";

            case RocketState_t::FIRST_PARACHUTE:
                return "FIRST_PARACHUTE";

            case RocketState_t::SECOND_PARACHUTE:
                return "SECOND_PARACHUTE";

            case RocketState_t::DROP:
                return "DROP";

            case RocketState_t::ABORT:
                return "ABORT";

            default:
                return "UNKNOWN";
        }
    }
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

RocketStateController::RocketStateController(): actual_state{RocketState_t::INIT}, prev_state{RocketState_t::INIT} {}

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

bool RocketStateController::SetState(const RocketState_t state) {
    std::lock_guard<std::mutex> lock_(mtx_);
    if (state == actual_state) {
        ara::log::LogInfo() << "Requested change to same state: " << to_string(state);
        return false;
    }
    if (!TransitionAllowed(this->actual_state, state)) {
        ara::log::LogWarn() << "Transition from: " << to_string(actual_state) <<
                                " To state: " << to_string(state) << "rejected";
        return false;
    }
    this->prev_state = actual_state;
    this->actual_state = state;

    on_change_callback_(actual_state);

    if (callbacks_.count(state)) {
        callbacks_[state]();
    }
    return true;
}

}  // namespace rocketState
}  // namespace core
}  // namespace srp
