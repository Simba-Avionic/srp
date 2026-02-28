/**
 * @file event_data.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-02-28
 * 
 * @copyright Copyright (c) 2026
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

// --- Szablony Pomocnicze ---

template <typename T>
void EventData::SetValue(T res, T* field) {
    std::unique_lock<std::shared_mutex> lock(this->mtx_);
    *field = res;
}

template <typename T>
T EventData::GetValue(const T* field) {
    std::shared_lock<std::shared_mutex> lock(this->mtx_);
    return *field;
}

// --- Stan Aktuatorów (Specyficzna logika bitowa) ---

uint8_t EventData::GetActuatorStates() {
    return GetValue(&this->actuator_state);
}

void EventData::SetActuatorState(const uint8_t actuator, const uint8_t state) {
    std::unique_lock<std::shared_mutex> lock(mtx_);
    if (state) {
        actuator_state |=  (1 << actuator);
    } else {
        actuator_state &= ~(1 << actuator);
    }
}

//  --- Stany Maszyny (EB/MB) ---

core::rocketState::RocketState_t EventData::GetEBState() {
    std::shared_lock<std::shared_mutex> lock(mtx_);
    return static_cast<core::rocketState::RocketState_t>(this->hb.engine_computer_state);
}

void EventData::SetEBState(const core::rocketState::RocketState_t state) {
    std::unique_lock<std::shared_mutex> lock(mtx_);
    this->hb.engine_computer_state = state;
}

core::rocketState::RocketState_t EventData::GetMBState() {
    std::shared_lock<std::shared_mutex> lock(mtx_);
    return static_cast<core::rocketState::RocketState_t>(this->hb.flight_computer_state);
}

void EventData::SetMBState(const core::rocketState::RocketState_t state) {
    std::unique_lock<std::shared_mutex> lock(mtx_);
    this->hb.flight_computer_state = state;
}

//  --- Temperatury Płyt (MB/EB) ---

void EventData::SetMBTemp(int16_t temp) { SetValue(temp, &this->board_temp.mb_temp); }
void EventData::SetEBTemp(int16_t temp) { SetValue(temp, &this->board_temp.eb_temp); }
int16_t EventData::GetMBTemp() { return GetValue(&this->board_temp.mb_temp); }
int16_t EventData::GetEBTemp() { return GetValue(&this->board_temp.eb_temp); }

//  --- Temperatury Zbiorników ---

void EventData::SetTemp1(int16_t res) { SetValue(res, &this->temp.upper_tank); }
void EventData::SetTemp2(int16_t res) { SetValue(res, &this->temp.middle_tank); }
void EventData::SetTemp3(int16_t res) { SetValue(res, &this->temp.lower_tank); }

uint16_t EventData::GetTemp1() { return GetValue(&this->temp.upper_tank); }
uint16_t EventData::GetTemp2() { return GetValue(&this->temp.middle_tank); }
uint16_t EventData::GetTemp3() { return GetValue(&this->temp.lower_tank); }

//  --- Ciśnienie i Wysokość ---

void EventData::SetPress(uint16_t res) { SetValue(res, &this->press.pressure); }
float EventData::GetPress() { return GetValue(&this->press.pressure); }

void EventData::SetMaxAltitude(const int32_t alt) { SetValue(alt, &this->max_altitude.altitude); }
int32_t EventData::GetMaxAltitude() { return GetValue(&this->max_altitude.altitude); }

//  --- GPS (Wielopolowy lock) ---

void EventData::SetGPS(int32_t lon, int32_t lat, int32_t alt) {
    std::unique_lock<std::shared_mutex> lock(this->mtx_);
    this->gps.lat = lat;
    this->gps.lon = lon;
    this->gps.altitude = alt;
}

int32_t EventData::GetGPSLat() { return GetValue(&this->gps.lat); }
int32_t EventData::GetGPSLon() { return GetValue(&this->gps.lon); }
int32_t EventData::GetGPSAlt() { return GetValue(&this->gps.altitude); }

//  --- Eksplicytne Instancje Szablonów ---

template void EventData::SetValue<uint16_t>(uint16_t, uint16_t*);
template void EventData::SetValue<int16_t>(int16_t, int16_t*);
template void EventData::SetValue<int32_t>(int32_t, int32_t*);
template void EventData::SetValue<float>(float, float*);
template void EventData::SetValue<uint8_t>(uint8_t, uint8_t*);

template uint16_t EventData::GetValue<uint16_t>(const uint16_t*);
template int16_t EventData::GetValue<int16_t>(const int16_t*);
template int32_t EventData::GetValue<int32_t>(const int32_t*);
template float EventData::GetValue<float>(const float*);
template uint8_t EventData::GetValue<uint8_t>(const uint8_t*);

}  // namespace apps
}  // namespace srp
