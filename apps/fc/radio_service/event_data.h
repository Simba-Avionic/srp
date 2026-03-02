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

class EventData {
 private:
  std::shared_mutex mtx_;

  mavlink_simba_tank_temperature_t temp;
  mavlink_simba_tank_pressure_t press;

  mavlink_simba_gps_t gps;
  mavlink_simba_imu_t imu;
  mavlink_simba_max_altitude_t max_altitude;

  mavlink_simba_rocket_heartbeat_t hb;
  uint8_t actuator_state;

  int16_t EB_Temp_sensors_data[3];
  int16_t MB_Temp_sensors_data[3];

  template <typename T>
  void SetValue(T res, T* field);

  template <typename T>
  T GetValue(const T* field);

 public:
  static std::shared_ptr<EventData> GetInstance();

  core::rocketState::RocketState_t GetEBState();
  void SetEBState(const core::rocketState::RocketState_t state);
  core::rocketState::RocketState_t GetMBState();
  void SetMBState(const core::rocketState::RocketState_t state);


  uint8_t GetActuatorStates();
  void SetActuatorState(const uint8_t actuator, const uint8_t state);

  void SetTemp1(int16_t res);
  void SetTemp2(int16_t res);
  void SetTemp3(int16_t res);

  void SetMBTemp(uint8_t sensor, int16_t temp);
  void SetEBTemp(uint8_t sensor, int16_t temp);
  int16_t GetMBTemp();
  int16_t GetEBTemp();

  void SetPress(uint16_t res);

  void SetGPS(int32_t lon, int32_t lat, int32_t alt);

  void SetMaxAltitude(const int32_t alt);

  int32_t GetMaxAltitude();

  uint16_t GetTemp1();
  uint16_t GetTemp2();
  uint16_t GetTemp3();

  float GetPress();

  int32_t GetGPSLat();
  int32_t GetGPSLon();
  int32_t GetGPSAlt();
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_EVENT_DATA_H_
