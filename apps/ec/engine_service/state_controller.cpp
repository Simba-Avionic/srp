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

namespace srp {
namespace engineApp {

namespace {
    static std::shared_ptr<EngineStateController> instance = nullptr;
}

EngineStateController::EngineStateController() {
    state_ = apps::RocketState_t::INIT;
}
std::shared_ptr<EngineStateController> EngineStateController::GetInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<EngineStateController>();
    }
    return instance;
}
apps::RocketState_t EngineStateController::GetEngineState() {
    std::lock_guard<std::mutex> lock(state_mtx_);
    return state_;
}
void EngineStateController::SetEngineState(const apps::RocketState_t state) {
    std::lock_guard<std::mutex> lock(state_mtx_);
    this->state_ = state;
}


}  // namespace engineApp
}  // namespace srp
