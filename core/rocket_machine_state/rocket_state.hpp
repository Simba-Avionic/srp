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
#include <string>

namespace srp {
namespace core {
namespace rocketState {

enum RocketState_t: uint8_t {
    INIT = 0,
    DISARM = 1,
    ARM = 2,
    LAUNCH = 3,
    FLIGHT = 4 ,
    APOGEE = 5,
    FIRST_PARACHUTE = 6,
    SECOND_PARACHUTE = 7,
    DROP = 8,
    ABORT = 64,
};

static std::string to_string(const RocketState_t state) {
    std::string res;
    switch (state) {
    case RocketState_t::INIT:
        res = "INIT";
        break;
    case RocketState_t::DISARM:
        res = "DISARM";
        break;
    case RocketState_t::ARM:
        res = "ARM";
        break;
    case RocketState_t::LAUNCH:
        res = "LAUNCH";
        break;
    case RocketState_t::FLIGHT:
        res = "FLIGHT";
        break;
    case RocketState_t::APOGEE:
        res = "APOGEE";
        break;
    case RocketState_t::FIRST_PARACHUTE:
        res = "FIRST_PARACHUTE";
        break;
    case RocketState_t::SECOND_PARACHUTE:
        res = "SECOND_PARACHUTE";
        break;
    case RocketState_t::ABORT:
        res = "ABORT";
        break;
    default:
        break;
    }
    return res;
}

using StateCallback = std::function<void()>;
using ChangeRequestCallback = std::function<bool(RocketState_t new_state)>;
using OnStateChangeCallback = std::function<void(RocketState_t new_state)>;

class RocketStateController {
 private:
    std::mutex mtx_;
    std::unordered_map<RocketState_t, StateCallback> callbacks_;
    RocketState_t prev_state;
    RocketState_t actual_state;

    // use for status update
    OnStateChangeCallback on_change_callback_;

    ChangeRequestCallback requirements_callback_;


    static bool TransitionAllowed(const RocketState_t actual_stete, const RocketState_t req_state);
 public:
    void RegisterRequirementsCallback(ChangeRequestCallback cb);
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
