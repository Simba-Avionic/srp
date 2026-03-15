/**
 * @file onTimer.hpp
 * @author Mateusz Krajewski (matikajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CORE_ONTIMERCALLBACK_ONTIMER_HPP_
#define CORE_ONTIMERCALLBACK_ONTIMER_HPP_
#include <chrono>  // NOLINT
#include <mutex>  // NOLINT
#include <thread>  // NOLINT
#include <stop_token>  // NOLINT
#include <functional>
#include <algorithm>
#include <vector>

namespace srp {
namespace core {

using OnTimerCallback = std::function<void()>;
using time_t =  std::chrono::time_point<std::chrono::high_resolution_clock>;

struct OnTimerCallback_t {
    const OnTimerCallback callback;
    time_t last_call;
    const uint32_t Interval_ms;
};


class TimerController {
 private:
    std::mutex timer_mtx_;
    std::vector<OnTimerCallback_t> timers;
    std::jthread loop_thread_;

    void Loop(const std::stop_token& token);
 public:
    TimerController(const TimerController&) = delete;
    TimerController& operator=(const TimerController&) = delete;
    TimerController(TimerController&&) = delete;
    TimerController& operator=(TimerController&&) = delete;

    void AddOnTimerCallback(OnTimerCallback callback, u_int32_t Interval_ms);
    void Start();
    void Stop();
};

}  // namespace core
}  // namespace srp

#endif  // CORE_ONTIMERCALLBACK_ONTIMER_HPP_
