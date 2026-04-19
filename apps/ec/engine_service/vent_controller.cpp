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
#include <algorithm>
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
    static constexpr auto kVent_required_req_frequency_ms = 1050;
}

void VentController::VentingLoop(const std::stop_token& t) {
    uint8_t vent_state = 1;
    while (!t.stop_requested()) {
        vent_handler(vent_state);
        vent_state = (vent_state == 1) ? 0 : 1;
        core::condition::wait_for(GetVentDelay(), t);
    }
}

void VentController::ChangeState(const VentState_e new_state) {
    std::lock_guard<std::mutex> lock_guard(mtx_);
    switch (new_state) {
    case VentState_e::OPEN:
        actual_state = VentState_e::OPEN;
        if (opening_thread) {
            opening_thread->request_stop();
            opening_thread->join();
        }
        vent_handler(1);
        break;
    case VentState_e::CLOSE:
        actual_state = VentState_e::CLOSE;
        if (opening_thread) {
            opening_thread->request_stop();
            opening_thread->join();
        }
        vent_handler(0);
        break;
    case VentState_e::OPENING:
        if (actual_state != VentState_e::OPENING) {
            last_requested_opening = std::chrono::high_resolution_clock::now();
            actual_state = VentState_e::OPENING;
        } else {
            last_requested_opening = std::max(last_requested_opening, std::chrono::high_resolution_clock::now());
        }
        opening_thread = std::make_unique<std::jthread>([this](const std::stop_token& t){
            VentingLoop(t);
        });
        break;
    default:
        break;
    }
}

VentController::VentController() {
    auto_close_thread = std::make_unique<std::jthread>([this](const std::stop_token& t) {
        while (!t.stop_requested()) {
            core::condition::wait_for(std::chrono::milliseconds(100), t);
            if (actual_state != VentState_e::OPENING) {
                continue;
            }
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_requested_opening);
            if (duration.count() >= kVent_required_req_frequency_ms) {
                ChangeState(VentState_e::CLOSE);
            }
        }
    });
}

}  // namespace engine
}  // namespace apps
}  // namespace srp
