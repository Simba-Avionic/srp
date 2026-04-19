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
    CONNECTION_LOST = 54,  // 2min od ostatniej wiadomosci przechodzimy w stan
        //  connection_lost (disarm), po x minutach tego stanu robimy abort

    ABORT = 64,            // ABORT -> spuszczamy paliwo i wylaczamy silniki
    INIT = 0,              // Domyślny stan
    DISARM = 1,            // Wyłączone actuatory
    ARM = 2,               // Actuatory zasilane
    LAUNCH = 3,            // Sekwencja startowa
    // STANY powyżej sterowane są wspólnie (dla obu komputerów przez GS)

    // Stan flight jest wywoływany dla fc i ec przez EC
    FLIGHT = 4 ,           // Lot przed apogeum

    // Stany poniżej wywoływane są przez fc na obu komputerach
    APOGEE = 5,            // Apogeum wyzwolone
    FIRST_PARACHUTE = 6,   // Opadanie zrefowany spadochron
    SECOND_PARACHUTE = 7,  // Sekwencja odrefowania spadochronu
    DROP = 8,              // Spadanie odrefowany spadochron
    TOUCHDOWN = 9,         // Rakieta na ziemi
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
    case RocketState_t::CONNECTION_LOST:
        res = "CONNECTION_LOST";
        break;
    default:
        res = "";
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
