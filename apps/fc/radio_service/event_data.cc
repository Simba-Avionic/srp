/**
 * @file event_data.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-26
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

template <typename T>
void EventData::SetValue(T res, T* field) {
  std::unique_lock<std::shared_mutex> lock(this->mtx_);
  *field = res;
}

void EventData::SetTemp1(uint16_t res) {
  SetValue(res, &this->temp.temp1);
}

void EventData::SetTemp2(uint16_t res) {
  SetValue(res, &this->temp.temp2);
}

void EventData::SetTemp3(uint16_t res) {
  SetValue(res, &this->temp.temp3);
}

void EventData::SetDPress(float res) {
  SetValue(res, &this->press.d_pressure);
}

void EventData::SetPress(float res) {
  SetValue(res, &this->press.pressure);
}

void EventData::SetGPS(int32_t lon, int32_t lat) {
  std::unique_lock<std::shared_mutex> lock(this->mtx_);
  this->gps.lat = lat;
  this->gps.lon = lon;
}

void EventData::SetActuatorBit(uint8_t res, uint8_t bit_position) {
  if (bit_position > 7 || res > 1) {
    return;
  }
  std::unique_lock<std::shared_mutex> lock(this->mtx_);
  this->actuator.values =
      (this->actuator.values & ~(1 << bit_position)) | (res << bit_position);
}

uint16_t EventData::GetTemp1() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->temp.temp1;
}

uint16_t EventData::GetTemp2() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->temp.temp2;
}

uint16_t EventData::GetTemp3() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->temp.temp3;
}

float EventData::GetDPress() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->press.d_pressure;
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

uint8_t EventData::GetActuator() {
  std::shared_lock<std::shared_mutex> lock(mtx_);
  return this->actuator.values;
}

// Instancja szablonu musi być jawnie zdefiniowana w pliku .cpp
template void EventData::SetValue<uint16_t>(uint16_t, uint16_t*);
template void EventData::SetValue<float>(float, float*);

}  // namespace apps
}  // namespace srp
