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
    static constexpr auto kFlow_check_interval_ms = 100;  // 10Hz
    static constexpr auto kValve_delay_ms = 1000;
    static constexpr auto kFlow_stopped_margin = 0.5;
    static constexpr double kMass_margin_multiplier = 1.05;
    static constexpr int16_t kPressure_tolerance = 100;

    template <typename T, size_t N> class MovingAveragePredictor {
    private:
        std::array<T, N> buffer{};
        std::array<std::chrono::steady_clock::time_point, N> timestamps{};
        size_t head{0};
        size_t count{0};

    public:
        void addSample(T val) {
            buffer[head] = val;
            timestamps[head] = std::chrono::steady_clock::now();
            head = (head + 1) % N;
            if (count < N) count++;
        }

        void reset() {
            buffer.fill(0);
            count = 0;
            head = 0;
        }

        bool isReady() const {
            return count >= 2;
        }

        double getChangeSpeed() const {
            if (!isReady()) return 0.0;
            size_t oldest_idx = (count < N) ? 0 : head;
            size_t newest_idx = (head + N - 1) % N;
            T oldest_val = buffer[oldest_idx];
            T newest_val = buffer[newest_idx];
            
            auto dt_total = std::chrono::duration<double>(timestamps[newest_idx] - timestamps[oldest_idx]).count();
            if (dt_total <= 0.0001) return 0.0;
            
            return static_cast<double>(newest_val - oldest_val) / dt_total;
        }

        /*
        * @brief Function calculates estimated parameter value in a current moment
        *        based on moving avarage and growth speed of this value in a time.
        * @return Estimated value of a parameter
        */
        double getProjectedValue() const {
            if (!isReady()) return buffer[(head + N - 1) % N];

            size_t oldest_idx = (count < N) ? 0 : head;
            size_t newest_idx = (head + N - 1) % N;

            T oldest_val = buffer[oldest_idx];
            T newest_val = buffer[newest_idx];

            double sum = 0.0;
            for (size_t i = 0; i < count; ++i) {
                sum += buffer[i];
            }
            double avg_val = sum / count;

            auto dt_total = std::chrono::duration<double>(timestamps[newest_idx] - timestamps[oldest_idx]).count();
            if (dt_total <= 0.0001) return avg_val;

            double speed = static_cast<double>(newest_val - oldest_val) / dt_total;

            double filter_lag_s = dt_total / 2.0;

            return avg_val + speed * filter_lag_s;
        }
    };
}  // namespace

    bool RefuelController::Initialize(Config conf, SetValvePosCallback main_v_set_pos, 
                    SetValvePosCallback vent_v_set_pos, SetValvePosCallback dump_v_set_pos,
                    CheckValvePosCallback vent_v_check_pos) {

        if (!main_v_set_pos || !dump_v_set_pos) {
            ara::log::LogError() << "Failed to initialize RefuelController: Missing main or dump callbacks!";
            return false;
        }

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

    void RefuelController::StartRefueling(RefuelingType_t type, uint16_t val) {
        {
            std::lock_guard<std::mutex> lock(cv_mtx_);
            target_refueling_value = val;
            refueling_type = type;
            initial_rocket_mass = rocket_mass.load();
            state = RefuelingState_t::PRESSURE_TEST;
        }
        cv.notify_all();
    }

    void RefuelController::changeState(RefuelingState_t new_state) {
        {
            std::lock_guard<std::mutex> lock(cv_mtx_);
            state = new_state;
        }
        cv.notify_all();
    }

    void RefuelController::handleIdle() {
        std::unique_lock<std::mutex> lock(cv_mtx_);

        cv.wait_for(lock, std::chrono::seconds(10), [this] {
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
        
        std::unique_lock<std::mutex> lock(cv_mtx_);
        
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

    void RefuelController::handleRefuelling() {
        ara::log::LogInfo() << "Starting refuelling for tank: " << config.tank_name;
        set_gs_main_valve_position(kGS_valve_position_open100);
        
        const auto start_time = std::chrono::steady_clock::now();
        const auto max_duration = std::chrono::seconds(config.max_tanking_time_s);
        
        MovingAveragePredictor<double, 10> flow_predictor;

        while (state == RefuelingState_t::REFUELING) {

            {
                std::unique_lock<std::mutex> lock(cv_mtx_);
                if (cv.wait_for(lock, std::chrono::milliseconds(kFlow_check_interval_ms), [this] { 
                    return state != RefuelingState_t::REFUELING; 
                })) {
                    break; 
                }
            }

            if (std::chrono::steady_clock::now() - start_time > max_duration) {
                ara::log::LogError() << "Max refuelling time exceeded! Aborting refueling sequence for tank: " << config.tank_name;
                Abort();
                return;
            }

            if (RefuelingType_t::MASS == refueling_type && check_vent_valve_position) {
                if (kServo_valve_position_closed != check_vent_valve_position()) {
                    if (set_vent_valve_position) {
                        set_vent_valve_position(kServo_valve_position_closed);
                    }
                    
                    std::unique_lock<std::mutex> lock(cv_mtx_);
                    if (cv.wait_for(lock, std::chrono::milliseconds(kValve_delay_ms), [this] { 
                        return state != RefuelingState_t::REFUELING; 
                    })) {
                        break;
                    }
                }
            }

            const int16_t current_mass = rocket_mass.load();
            const uint16_t current_tank_press = tank_pressure.load();
            const uint16_t current_gs_press = gs_pressure.load();

            const double loaded_mass = static_cast<double>(current_mass - initial_rocket_mass);

            if (refueling_type == RefuelingType_t::MASS) {
                flow_predictor.addSample(loaded_mass);
            } else {
                flow_predictor.addSample(static_cast<double>(current_tank_press));
            }

            if (!flow_predictor.isReady()) {
                continue;
            }

            const double projected_val = flow_predictor.getProjectedValue();
            const double flow_rate = flow_predictor.getChangeSpeed();

            if (refueling_type == RefuelingType_t::MASS) {
                if (projected_val >= target_refueling_value * kMass_margin_multiplier) {
                    ara::log::LogInfo() << "Target mass reached. Projected net loaded: " << projected_val;
                    changeState(RefuelingState_t::TANKING_COMPLETE);
                    return;
                }

                bool press_equalized = std::abs(current_tank_press - current_gs_press) <= kPressure_tolerance;
                bool no_mass_flow = (flow_rate < kFlow_stopped_margin); 

                if (press_equalized && no_mass_flow) {
                    if (set_vent_valve_position) {
                        ara::log::LogInfo() << "Flow stopped. Pulsing vent valve to drop pressure.";
                        set_vent_valve_position(kServo_valve_position_open);
                        
                        {
                            std::unique_lock<std::mutex> lock(cv_mtx_);
                            if (cv.wait_for(lock, std::chrono::milliseconds(1500), [this] { 
                                return state != RefuelingState_t::REFUELING; 
                            })) {
                                break;
                            }
                        }
                        
                        set_vent_valve_position(kServo_valve_position_closed);
                        
                        {
                            std::unique_lock<std::mutex> lock(cv_mtx_);
                            if (cv.wait_for(lock, std::chrono::milliseconds(kValve_delay_ms), [this] { 
                                return state != RefuelingState_t::REFUELING; 
                            })) {
                                break;
                            }
                        }
                        
                        flow_predictor.reset();
                    } else {
                        ara::log::LogWarn() << "Mass flow stopped, but NO vent valve is configured to drop pressure! Waiting...";
                    }
                }
            } else if (refueling_type == RefuelingType_t::PRESSURE) {
                if (projected_val >= target_refueling_value) {
                    ara::log::LogInfo() << "Target pressure reached. Projected: " << projected_val;
                    changeState(RefuelingState_t::TANKING_COMPLETE);
                    return;
                }
                
                bool no_press_flow = (flow_rate < kFlow_stopped_margin);
                                    
                if (no_press_flow && std::abs(current_tank_press - current_gs_press) <= kPressure_tolerance) {
                    ara::log::LogWarn() << "Pressure flow stopped due to equalization, but target not reached.";
                }
            }
        }
    }

    void RefuelController::handleTankingComplete() {
        set_gs_main_valve_position(kGS_valve_position_closed);
        ara::log::LogInfo() << "Tanking complete for tank: " << config.tank_name;

        while (state.load() == RefuelingState_t::TANKING_COMPLETE) {
            if (tank_pressure.load() > config.max_safe_pressure) {
                ara::log::LogWarn() << "Tank overpressure detected. Venting...";

                if (set_vent_valve_position) {
                    set_vent_valve_position(kServo_valve_position_open);

                    {
                        std::unique_lock<std::mutex> lock(cv_mtx_);
                        if (cv.wait_for(lock, std::chrono::milliseconds(1500), [this] { return state != RefuelingState_t::TANKING_COMPLETE; })) {
                            break;
                        }
                    }
                    
                    set_vent_valve_position(kServo_valve_position_closed);
                } else {
                    ara::log::LogWarn() << "Overpressure detected, but NO vent valve available!";
                }
            }
            {
                std::unique_lock<std::mutex> lock(cv_mtx_);
                if (cv.wait_for(lock, std::chrono::milliseconds(1000), [this] { return state != RefuelingState_t::TANKING_COMPLETE; })) {
                    break;
                }
            }
        }
    }

    void RefuelController::RequestAbort() {
        ara::log::LogInfo() << "Requested to abort refuelling of tank: " << config.tank_name << " from remote.";
        Abort();
    }

    void RefuelController::Abort() {
        changeState(RefuelingState_t::ABORT);

        set_gs_main_valve_position(kGS_valve_position_closed);
        if (set_vent_valve_position) { set_vent_valve_position(kServo_valve_position_closed); }
        set_dump_valve_position(kServo_valve_position_open);

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
                    handleRefuelling();
                    break;
                case RefuelingState_t::TANKING_COMPLETE:
                    handleTankingComplete();
                    break;
                case RefuelingState_t::ABORT:
                    Abort();
                    break;
                default:
                    ara::log::LogError() << "Refueling state: " << to_string(state) << "not handled";
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    break;
            }
        }
        
    }

}  // namespace apps
}  // namespace srp
