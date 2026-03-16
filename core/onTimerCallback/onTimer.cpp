/**
 * @file onTimer.cpp
 * @author Mateusz Krajewski (matikajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "core/onTimerCallback/onTimer.hpp"

#include <algorithm>

#include "core/common/condition.h"

namespace srp {
namespace core {


void TimerController::Loop(const std::stop_token& token) {
    while (!token.stop_requested()) {
        uint32_t can_sleep_for_ms = 1000;
        auto now = std::chrono::high_resolution_clock::now();
        std::vector<OnTimerCallback> callbacks_to_run;
        {
            std::lock_guard<std::mutex> lock(timer_mtx_);
            for (auto &timer : timers) {
                const auto elapsed = std::chrono::duration_cast<
                std::chrono::milliseconds>(now - timer.last_call).count();
                if (elapsed > timer.Interval_ms) {
                    timer.last_call += std::chrono::milliseconds(timer.Interval_ms);
                    callbacks_to_run.push_back(timer.callback);
                } else {
                    uint32_t time_to_next_callback = timer.Interval_ms - elapsed;
                    can_sleep_for_ms = std::min(can_sleep_for_ms, time_to_next_callback);
                }
            }
        }
        for (auto& cb : callbacks_to_run) {
            cb();
        }

        condition::wait_for(std::chrono::milliseconds(can_sleep_for_ms), token);
    }
}

void TimerController::AddOnTimerCallback(OnTimerCallback callback, u_int32_t Interval_ms) {
    std::lock_guard<std::mutex> lock(timer_mtx_);
    OnTimerCallback_t cb = {
        .callback = callback,
        .last_call = std::chrono::high_resolution_clock::now(),
        .Interval_ms = Interval_ms};
    timers.push_back(cb);
}

void TimerController::Start() {
    loop_thread_ = std::jthread([this](const std::stop_token& token) {
        Loop(token);
    });
}

void TimerController::Stop() {
    if (loop_thread_.joinable()) {
        loop_thread_.request_stop();
        loop_thread_.join();
    }
}

}  // namespace core
}  // namespace srp
