/**
 * @file vent_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "apps/ec/engine_service/vent_controller.hpp"
#include <memory>
#include <functional>
#include "core/common/condition.h"
namespace srp {
namespace apps {
namespace engine {
namespace {
    static std::shared_ptr<VentController> instance = nullptr;
    static constexpr auto kVent_open_time_ms = 100;
    static constexpr auto kVent_opening_time_ms = 50;
    consteval std::chrono::milliseconds GetVentDelay() {
        return std::chrono::milliseconds(kVent_open_time_ms + kVent_opening_time_ms);
    }
}

std::shared_ptr<VentController> VentController::GetInstance() {
    if (!instance) {
        instance = std::make_shared<VentController>();
    }
    return instance;
}
void VentController::VentingLoop(const std::stop_token& t) {
    uint8_t vent_state = 1;
    while (!t.stop_requested()) {
        vent_handler(vent_state);
        vent_state = (vent_state == 1) ? 0 : 1;
        core::condition::wait_for(GetVentDelay(), t);
    }
}

bool VentController::ChangeState(const VentState_e new_state) {
    switch (new_state) {
    case VentState_e::OPEN:
        if (opening_thread) {
            opening_thread->request_stop();
            opening_thread->join();
        }
        vent_handler(1);
        break;
    case VentState_e::CLOSE:
        if (opening_thread) {
            opening_thread->request_stop();
            opening_thread->join();
        }
        vent_handler(0);
        break;
    case VentState_e::OPENING:
        opening_thread = std::make_unique<std::jthread>([this](const std::stop_token& t){
            VentingLoop(t);
        });
        break;
    default:
        break;
    }
}

}  // namespace engine
}  // namespace apps
}  // namespace srp
