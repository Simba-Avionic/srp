/**
 * @file refuel_controller.hpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_EC_FUEL_SERVICE_CONTROLLER_REFUEL_CONTROLLER_HPP_
#define APPS_EC_FUEL_SERVICE_CONTROLLER_REFUEL_CONTROLLER_HPP_

#include <mutex>  // NOLINT
#include <thread>  // NOLINT
#include <chrono>
#include <functional>
#include <cstdint>
#include <string>
#include <atomic>
#include <condition_variable>

namespace srp {
namespace apps {

using SetValvePosCallback = std::function<void(const uint8_t pos)>;
using CheckValvePosCallback = std::function<uint8_t()>;

enum RefuelingState_t: uint8_t {
    ABORT = 64,
    INIT = 0,
    IDLE = 1,
    PRESSURE_TEST = 2,
    REFUELING = 3,
    TANKING_COMPLETE = 4
};

enum RefuelingType_t: uint8_t {
    MASS = 0,
    PRESSURE = 1
};

static std::string to_string(const RefuelingState_t state) {
    switch (state) {
    case RefuelingState_t::INIT:
        return "INIT";
    case RefuelingState_t::IDLE:
        return "IDLE";
    case RefuelingState_t::PRESSURE_TEST:
        return "PRESSURE_TEST";
    case RefuelingState_t::REFUELING:
        return "REFUELING";
    case RefuelingState_t::TANKING_COMPLETE:
        return "TANKING_COMPLETE";
    case RefuelingState_t::ABORT:
        return "ABORT";
    default:
        return "";
    }
}

struct Config {
    std::string tank_name;
    uint16_t min_gs_pressure;
    uint16_t max_gs_pressure;
    uint16_t test_pressure;
    uint8_t test_max_drop_percent;
    uint16_t max_safe_pressure;
    uint16_t max_tanking_time_s;
};

class RefuelController final {
 private:
    std::atomic<RefuelingState_t> state{RefuelingState_t::INIT};
    std::mutex state_mtx_;
    std::jthread refueling_thread;
    std::condition_variable cv;
    std::atomic<bool> running{false};

    RefuelingType_t refueling_type;
    uint16_t target_refueling_value;
    uint16_t start_test_pressure;

    std::atomic<uint16_t> tank_pressure{0};
    std::atomic<uint16_t> gs_pressure{0};
    std::atomic<int16_t> rocket_mass{0};
    
    Config config;
    SetValvePosCallback set_gs_main_valve_position;
    SetValvePosCallback set_vent_valve_position;
    SetValvePosCallback set_dump_valve_position;
    CheckValvePosCallback check_vent_valve_position;

    void RefuelingLoop(std::stop_token token);
    void Abort();

    void handleIdle();
    void handlePressureTest();
    void handleRefuelling();
 protected:
    void changeState(RefuelingState_t state);
 public:
    bool Initialize(Config config, SetValvePosCallback main_v_set_pos,
            SetValvePosCallback vent_v_set_pos, SetValvePosCallback dump_v_set_pos,
            CheckValvePosCallback vent_v_check_pos);

    void StartRefuelingToMass(uint16_t val);
    void StartRefuelingToPressure(uint16_t val);

    void OnTankPressureReceived(uint16_t val);
    void OnGSPressureReceived(uint16_t val);
    void OnRocketMassReceived(int16_t val);
};

}  // namespace apps
}  // namespace srp


#endif  // APPS_EC_FUEL_SERVICE_CONTROLLER_REFUEL_CONTROLLER_HPP_