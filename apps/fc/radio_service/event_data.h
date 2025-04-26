#ifndef APPS_FC_RADIO_SERVICE_EVENT_DATA_H_
#define APPS_FC_RADIO_SERVICE_EVENT_DATA_H_

#include <mutex>
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
  __mavlink_simba_actuator_t actuator;
  std::mutex mtx_;

  template <typename T>
  void SetValue(T res, T* field);

 public:
  static std::shared_ptr<EventData> GetInstance();

  void SetTemp1(uint16_t res);
  void SetTemp2(uint16_t res);
  void SetTemp3(uint16_t res);

  void SetDPress(float res);
  void SetPress(float res);

  void SetGPS(int32_t lon, int32_t lat);
  void SetActuatorBit(uint8_t res, uint8_t bit_position);

  uint16_t GetTemp1();
  uint16_t GetTemp2();
  uint16_t GetTemp3();

  float GetDPress();
  float GetPress();

  int32_t GetGPSLat();
  int32_t GetGPSLon();

  uint8_t GetActuator();
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_EVENT_DATA_H_
