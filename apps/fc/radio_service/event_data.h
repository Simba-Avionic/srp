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

#include "lib/simba/mavlink.h"

namespace srp {
namespace apps {

class EventData {
 private:
  __mavlink_simba_tank_temperature_t temp;
  __mavlink_simba_tank_pressure_t press;
  __mavlink_simba_gps_t gps;
  __mavlink_simba_imu_t imu;
  __mavlink_simba_max_altitude_t max_altitude;
  SIMBA_ROCKET_STATE rocket_state;
  uint8_t actuator_state;
  std::shared_mutex mtx_;

  template <typename T>
  void SetValue(T res, T* field);

 public:
  static std::shared_ptr<EventData> GetInstance();

  SIMBA_ROCKET_STATE GetRocketState();
  void SetRocketState(const SIMBA_ROCKET_STATE state);

  uint8_t GetActuatorStates();
  void SetActuatorState(const uint8_t actuator, const uint8_t state);

  void SetTemp1(int16_t res);
  void SetTemp2(int16_t res);
  void SetTemp3(int16_t res);

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
