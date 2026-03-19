/**
 * @file event_data.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_FC_RADIO_SERVICE_EVENT_DATA_H_
#define APPS_FC_RADIO_SERVICE_EVENT_DATA_H_

#include <shared_mutex>
#include <mutex>  // NOLINT
#include <memory>
#include <cstdint>
#include "simba/mavlink.h"
#include "core/rocket_machine_state/rocket_state.hpp"

namespace srp {
namespace apps {

template <typename T>
class ThreadSafe {
 public:
    void set(const T& value) {
        std::unique_lock lock(mtx_);
        data_ = value;
    }

    T get() const {
        std::shared_lock lock(mtx_);
        return data_;
    }

    void update(std::function<void(T&)> func) {
        std::unique_lock lock(mtx_);
        func(data_);
    }

 private:
    mutable std::shared_mutex mtx_;
    T data_{};
};

enum class BoardType_e {EB, MB};

class EventData {
 private:
    ThreadSafe<mavlink_simba_tank_temperature_t> temp_;
    ThreadSafe<mavlink_simba_tank_pressure_t> press_;
    ThreadSafe<mavlink_simba_gps_t> gps_;
    ThreadSafe<mavlink_simba_imu_t> imu_;
    ThreadSafe<mavlink_simba_max_altitude_t> max_altitude_;
    ThreadSafe<uint8_t> actuator_state_;

    ThreadSafe<core::rocketState::RocketState_t> EBState_;
    ThreadSafe<core::rocketState::RocketState_t> MBState_;

    ThreadSafe<std::array<int16_t, 3>> eb_temp_sensors_;
    ThreadSafe<std::array<int16_t, 3>> mb_temp_sensors_;

 public:
  static std::shared_ptr<EventData> GetInstance();

  core::rocketState::RocketState_t GetComputerState(const BoardType_e BoardType);
  void SetComputerState(const BoardType_e BoardType, const core::rocketState::RocketState_t state);


  uint8_t GetActuatorStates();
  void SetActuatorState(const uint8_t actuator_mask, const uint8_t state);

  uint16_t GetTemp(const uint8_t index);
  void SetTemp(const uint8_t index, const int16_t res);

  int16_t GetComputerTemp(const BoardType_e BoardType);
  void SetComputerTemp(const BoardType_e BoardType, const uint8_t index, const int16_t& temp);

  uint16_t GetPress();
  void SetPress(uint16_t res);

  mavlink_simba_gps_t GetGPS();
  void SetGPS(int32_t lon, int32_t lat, int32_t alt);

  int32_t GetMaxAltitude();
  void SetMaxAltitude(const int32_t alt);
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_EVENT_DATA_H_
