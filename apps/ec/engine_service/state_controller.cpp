/**
 * @file state_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-12-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "apps/ec/engine_service/state_controller.hpp"

#include <mutex>  // NOLINT
#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "ara/log/log.h"

namespace srp {
namespace engineApp {

namespace {
    static std::shared_ptr<EngineStateController> instance = nullptr;
    static const std::unordered_map<RocketState_t, std::vector<RocketState_t>> allowed_transitions = {
        {RocketState_t::INIT,                     {RocketState_t::DISARM, RocketState_t::ABORT}},
        {RocketState_t::DISARM,                   {RocketState_t::ARM, RocketState_t::ABORT}},
        {RocketState_t::ARM,                      {RocketState_t::LAUNCH, RocketState_t::ABORT, RocketState_t::DISARM}},
        {RocketState_t::ABORT,                    {RocketState_t::DISARM}},
        {RocketState_t::LAUNCH,                   {RocketState_t::FLIGHT}},
        {RocketState_t::FLIGHT,                   {RocketState_t::MAIN_PARACHUTE_TRIGGERED}},
        {RocketState_t::MAIN_PARACHUTE_TRIGGERED, {RocketState_t::SECOND_PARACHUTE_TRIGGERED}}};

}  // namespace

EngineStateController::EngineStateController() {
    state_ = apps::RocketState_t::INIT;
}
std::shared_ptr<EngineStateController> EngineStateController::GetInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<EngineStateController>();
    }
    return instance;
}
RocketState_t EngineStateController::GetEngineState() {
    std::lock_guard<std::mutex> lock(state_mtx_);
    return state_;
}
bool EngineStateController::SetEngineState(const RocketState_t state) {
    std::lock_guard<std::mutex> lock(state_mtx_);
    if (IsChangeStateAllowed(state)) {
        this->state_ = state;
        return true;
    }
    return false;
}

bool EngineStateController::IsChangeStateAllowed(const RocketState_t new_state) {
    if (new_state == this->state_) {
        return false;
    }
    const auto it = allowed_transitions.find(this->state_);
    if (it == allowed_transitions.end()) {
        return false;
    }
    if (std::find(it->second.begin(), it->second.end(), new_state) == it->second.end()) {
        return false;
    }
    return true;
}

}  // namespace engineApp
}  // namespace srp
