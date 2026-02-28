/**
 * @file event_data.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @author Michał Mańkowski ()
 * @brief 
 * @version 0.2
 * @date 2025-12-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "apps/fc/radio_service/event_data.h"

namespace srp {
namespace apps {
namespace {
static std::shared_ptr<EventData> event_data = nullptr;
}
std::shared_ptr<EventData> EventData::GetInstance() {
  if (event_data == nullptr) {
    event_data = std::make_shared<EventData>();
  }
  return event_data;
}

uint8_t EventData::GetActuatorStates() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->actuator_state;
}

void EventData::SetActuatorState(const uint8_t actuator, const uint8_t state) {
  std::unique_lock<std::shared_mutex> lock(mtx_);
  if (state) {
    actuator_state |=  (1 << actuator);
  } else {
    actuator_state &= ~(1 << actuator);
  }
}

SIMBA_ROCKET_STATE EventData::GetRocketState() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->rocket_state;
}

void EventData::SetRocketState(const SIMBA_ROCKET_STATE state) {
  SetValue(state, &this->rocket_state);
}


template <typename T>
void EventData::SetValue(T res, T* field) {
  std::unique_lock<std::shared_mutex> lock(this->mtx_);
  *field = res;
}

void EventData::SetTemp1(int16_t res) {
  SetValue(res, &this->temp.upper_tank);
}
void EventData::SetTemp2(int16_t res) {
  SetValue(res, &this->temp.middle_tank);
}

void EventData::SetTemp3(int16_t res) {
  SetValue(res, &this->temp.lower_tank);
}

void EventData::SetPress(uint16_t res) {
  SetValue(res, &this->press.pressure);
}

void EventData::SetGPS(int32_t lon, int32_t lat, int32_t alt) {
  std::unique_lock<std::shared_mutex> lock(this->mtx_);
  this->gps.lat = lat;
  this->gps.lon = lon;
  this->gps.altitude = alt;
}

uint16_t EventData::GetTemp1() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->temp.upper_tank;
}

uint16_t EventData::GetTemp2() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->temp.middle_tank;
}

uint16_t EventData::GetTemp3() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->temp.lower_tank;
}

float EventData::GetPress() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->press.pressure;
}

int32_t EventData::GetGPSLat() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->gps.lat;
}

int32_t EventData::GetGPSLon() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->gps.lon;
}
int32_t EventData::GetGPSAlt() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->gps.altitude;
}

void EventData::SetMaxAltitude(const int32_t alt) {
  SetValue(alt, &this->max_altitude.altitude);
}

int32_t EventData::GetMaxAltitude() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->max_altitude.altitude;
}

// Instancja szablonu musi być jawnie zdefiniowana w pliku .cpp
template void EventData::SetValue<uint16_t>(uint16_t, uint16_t*);
template void EventData::SetValue<float>(float, float*);

}  // namespace apps
}  // namespace srp
