/**
 * @file event_data.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_FC_RADIO_SERVICE_EVENT_DATA_H_
#define APPS_FC_RADIO_SERVICE_EVENT_DATA_H_

#include <mutex>  // NOLINT
#include <shared_mutex>
#include <memory>

#include "lib/simba/mavlink.h"
#include "ara/core/result.h"

namespace srp {
namespace apps {
class EventData;
static std::shared_ptr<EventData> event_data = nullptr;

class EventData {
 private:
  __mavlink_simba_tank_temperature_t temp;
  __mavlink_simba_tank_pressure_t press;
  __mavlink_simba_gps_t gps;
  __mavlink_simba_actuator_t actuator;
  std::mutex mtx_;
  template <typename T>
  void SetValue(T res, T* field) {
    std::unique_lock<std::mutex> lock(this->mtx_);
    *field = res;
  }

 public:
  static std::shared_ptr<EventData> GetInstance() {
  if (event_data == nullptr) {
    event_data = std::make_shared<EventData>();
  }
  return event_data;
}

  void SetTemp1(uint16_t res) {
     SetValue(res, &this->temp.temp1);
  }

  void SetTemp2(uint16_t res) {
    SetValue(res, &this->temp.temp2);
  }

  void SetTemp3(uint16_t res) {
    SetValue(res, &this->temp.temp3);
  }

  void SetDPress(float res) {
    SetValue(res, &this->press.Dpressure);
  }

  void SetPress(float res) {
    SetValue(res, &this->press.pressure);
  }

  void SetGPS(int32_t lon, int32_t lat) {
    std::unique_lock<std::mutex> lock(this->mtx_);
    this->gps.lat = lat;
    this->gps.lon = lon;
  }



  void SetActuatorBit(const std::uint8_t res, uint8_t bit_position) {
    if (bit_position > 7 || bit_position < 0 || res > 1) {
      return;
    }
    std::unique_lock<std::mutex> lock(this->mtx_);
    this->actuator.values = (this->actuator.values & ~(1 << bit_position)) | (res << bit_position);
  }

  uint16_t GetTemp1() {
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->temp.temp1;
  }

  uint16_t GetTemp2() {
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->temp.temp2;
  }

  uint16_t GetTemp3() {
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->temp.temp3;
  }

  float GetDPress() {
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->press.Dpressure;
  }

  float GetPress() {
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->press.pressure;
  }

  int32_t GetGPSLat() {
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->gps.lat;
  }

  int32_t GetGPSLon() {
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->gps.lon;
  }

  uint8_t GetActuator() {
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->actuator.values;
  }
};
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_EVENT_DATA_H_
