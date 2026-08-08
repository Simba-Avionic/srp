/**
 * @file refuel_controller.cpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #include <condition.h>
 #include "refuel_controller.hpp"
 #include "ara/log/log.h"

namespace srp {
namespace apps{
namespace {
    static constexpr auto kPressure_test_time_s = 30;
    static constexpr auto kPressure_stabilization_time_ms = 1000;
    static constexpr auto kGS_valve_position_closed = 0;
    static constexpr auto kGS_valve_position_open100 = 1;
    static constexpr auto kGS_valve_position_open50 = 2;
    static constexpr auto kServo_valve_position_closed = 0u;
    static constexpr auto kServo_valve_position_open = 1u;
}  // namespace

    bool RefuelController::Initialize(Config conf, SetValvePosCallback main_v_set_pos, 
                    SetValvePosCallback vent_v_set_pos, SetValvePosCallback dump_v_set_pos,
                    CheckValvePosCallback vent_v_check_pos) {
        config = conf;
        set_gs_main_valve_position = main_v_set_pos;
        set_vent_valve_position = vent_v_set_pos;
        set_dump_valve_position = dump_v_set_pos;
        check_vent_valve_position = vent_v_check_pos;

        ara::log::LogInfo() << "Refueling Controller initialized for tank: " << config.tank_name;
        
        changeState(RefuelingState_t::IDLE);
        running = true;

        refueling_thread = std::jthread([this](std::stop_token token){
            RefuelingLoop(token);
        });

        ara::log::LogInfo() << "Starting refueling thread for tank: " << config.tank_name;

        return true;
    }

    void RefuelController::StartRefuelingToMass(uint16_t val) {
        {
            std::unique_lock<std::mutex> lock(state_mtx_);

            target_refueling_value = val;
            refueling_type = RefuelingType_t::MASS;

            state = RefuelingState_t::PRESSURE_TEST;
        }
        cv.notify_all();
    }

    void RefuelController::StartRefuelingToPressure(uint16_t val) {
        {
            std::unique_lock<std::mutex> lock(state_mtx_);

            target_refueling_value = val;
            refueling_type = RefuelingType_t::PRESSURE;

            state = RefuelingState_t::PRESSURE_TEST;
        }
        cv.notify_all();
    }

    void RefuelController::changeState(RefuelingState_t new_state) {
        {
            std::unique_lock<std::mutex> lock(state_mtx_);
            state = new_state;
        }
        cv.notify_all();
    }

    void RefuelController::handleIdle() {
        std::unique_lock<std::mutex> lock(state_mtx_);

        bool condition_met = cv.wait_for(lock, std::chrono::seconds(10), [this] {
            const auto p = gs_pressure.load();
            bool press_invalid = (p < config.min_gs_pressure || p > config.max_gs_pressure);
            bool state_changed = (state != RefuelingState_t::IDLE);
            
            return press_invalid || state_changed;
        });

        const auto current_press = gs_pressure.load();
        if (current_press < config.min_gs_pressure || current_press > config.max_gs_pressure) {
            ara::log::LogError() << "GS pressure value: " << current_press << " is out of range ("
                << config.min_gs_pressure << " - " << config.max_gs_pressure << "). Aborting refueling sequence of tank: "
                << config.tank_name << ".";

            lock.unlock();
            Abort();
            return;
        }
        
        if (state == RefuelingState_t::IDLE) {
            ara::log::LogInfo() << "Waiting to start tanking sequence. Tank: " << config.tank_name;
        }
    }

    void RefuelController::handlePressureTest() {
        ara::log::LogInfo() << "Starting pressure test for tank: " << config.tank_name;
        set_gs_main_valve_position(kGS_valve_position_open50);
        
        std::unique_lock<std::mutex> lock(state_mtx_);
        
        bool target_reached = cv.wait_for(lock, std::chrono::seconds(config.max_tanking_time_s), [this] {
            return tank_pressure.load() >= config.test_pressure || state != RefuelingState_t::PRESSURE_TEST;
        });

        if (state != RefuelingState_t::PRESSURE_TEST) {
            return;
        }

        if (!target_reached || tank_pressure.load() < config.test_pressure) {
            ara::log::LogError() << "Failed to reach test pressure within timeout!";
            lock.unlock();
            Abort();
            return;
        }

        set_gs_main_valve_position(kGS_valve_position_closed);

        ara::log::LogInfo() << "Target pressure reached during pressure test for tank: " << config.tank_name;
        
        bool aborted = cv.wait_for(lock, std::chrono::milliseconds(kPressure_stabilization_time_ms), [this] {
            return state != RefuelingState_t::PRESSURE_TEST;
        });

        if (state != RefuelingState_t::PRESSURE_TEST) {
            return;
        }

        start_test_pressure = tank_pressure.load();

        bool leaked_or_state_change = cv.wait_for(lock, std::chrono::seconds(kPressure_test_time_s), [this] {
            return tank_pressure.load() < start_test_pressure * ((100.0 - config.test_max_drop_percent) / 100.0) || 
                state != RefuelingState_t::PRESSURE_TEST;
        });

        if (state != RefuelingState_t::PRESSURE_TEST) {
            return;
        }

        const double drop_limit = start_test_pressure * ((100.0 - config.test_max_drop_percent) / 100.0);
        if (tank_pressure.load() < drop_limit) {
            ara::log::LogError() << "Pressure drop detected during pressure test! Aborting.";
            lock.unlock();
            Abort();
            return;
        }

        ara::log::LogInfo() << "Pressure test passed for tank: " << config.tank_name << ". Starting main refueling process.";

        lock.unlock();
        changeState(RefuelingState_t::REFUELING);
    }

    void RefuelController::Abort() {
        changeState(RefuelingState_t::ABORT);

        set_gs_main_valve_position(kGS_valve_position_closed);
        set_vent_valve_position(kServo_valve_position_closed);

        running = false;

        ara::log::LogError() << "Refueling of tank: " << config.tank_name << " aborted!";
    }

    void RefuelController::OnTankPressureReceived(uint16_t val) {
        tank_pressure.store(val);
        cv.notify_all();
    }
    void RefuelController::OnGSPressureReceived(uint16_t val) {
        gs_pressure.store(val);
        cv.notify_all();
    }
    void RefuelController::OnRocketMassReceived(int16_t val) {
        rocket_mass.store(val);
        cv.notify_all();
    }

    void RefuelController::RefuelingLoop(std::stop_token token) {

        if (state == RefuelingState_t::ABORT) {
            Abort();
        }

        while (running && !token.stop_requested()) {
            switch (state)
            {
            case RefuelingState_t::IDLE:
                handleIdle();
                break;
            case RefuelingState_t::PRESSURE_TEST:
                handlePressureTest();
                break;
            case RefuelingState_t::REFUELING:
                break;
            case RefuelingState_t::TANKING_COMPLETE:
                break;
            default:
                Abort();
                break;
            }
        }
        
    }

}  // namespace apps
}  // namespace srp
