/**
 * @file rocket_state.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-02-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef CORE_ROCKET_MACHINE_STATE_ROCKET_STATE_HPP_
#define CORE_ROCKET_MACHINE_STATE_ROCKET_STATE_HPP_

#include <mutex>  // NOLINT
#include <memory>
#include <unordered_map>
#include <functional>

namespace srp {
namespace core {
namespace rocketState {

enum RocketState_t: uint8_t {
    INIT = 0,
    DISARM,
    ARM,
    LAUNCH,
    FLIGHT,
    APOGEE,
    FIRST_PARACHUTE,
    SECOND_PARACHUTE,
    DROP,
    ABORT = 64,
};

using StateCallback = std::function<void()>;
using OnStateChangeCallback = std::function<void(RocketState_t new_state)>;

class RocketStateController {
 private:
    std::mutex mtx_;
    std::unordered_map<RocketState_t, StateCallback> callbacks_;
    RocketState_t prev_state;
    RocketState_t actual_state;

    // use for status update
    OnStateChangeCallback on_change_callback_;

    static bool TransitionAllowed(const RocketState_t actual_stete, const RocketState_t req_state);
 public:
    void RegisterCallback(RocketState_t state, StateCallback cb);
    void RegisterOnStateChangeCallback(OnStateChangeCallback cb);
    static std::shared_ptr<RocketStateController> GetInstance();
    RocketState_t GetState();
    bool SetState(const RocketState_t state);
    RocketStateController();
};

}  // namespace rocketState
}  // namespace core
}  // namespace srp


#endif  // CORE_ROCKET_MACHINE_STATE_ROCKET_STATE_HPP_
