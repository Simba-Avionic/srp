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

#include <algorithm>
#include <numeric>

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

  core::rocketState::RocketState_t EventData::GetComputerState(const BoardType_e BoardType) {
    switch (BoardType) {
      case BoardType_e::EB:
        return EBState_.get();
      case BoardType_e::MB:
        return MBState_.get();
      default:
        return core::rocketState::RocketState_t::INIT;
    }
  }

  void EventData::SetComputerState(const BoardType_e BoardType, const core::rocketState::RocketState_t state) {
    switch (BoardType) {
      case BoardType_e::EB:
        EBState_.set(state);
        break;
      case BoardType_e::MB:
        MBState_.set(state);
        break;
      default:
        break;
    }
  }

  uint8_t EventData::GetPrimerStates() {
    return primer_state_.get();
  }
  void EventData::SetPrimerState(const uint8_t state) {
    primer_state_.set(state);
  }


  uint8_t EventData::GetActuatorStates() {
    return actuator_state_.get();
  }
  void EventData::SetActuatorState(const uint8_t actuator_mask, const uint8_t state) {
    actuator_state_.update([&](uint8_t& current_mask) {
      if (state) {
        current_mask |= actuator_mask;
      } else {
        current_mask &= static_cast<uint8_t>(~actuator_mask);
      }
    });
  }

  int16_t EventData::GetComputerTemp(const BoardType_e BoardType) {
    if (BoardType == BoardType_e::EB) {
        const auto temps = eb_temp_sensors_.get();
        if (temps.empty()) return 0;

        int32_t sum = std::accumulate(temps.begin(), temps.end(), 0);
        return static_cast<int16_t>(sum / temps.size());
    }
    if (BoardType == BoardType_e::MB) {
        const auto temps = mb_temp_sensors_.get();
        if (temps.empty()) return 0;

        int32_t sum = std::accumulate(temps.begin(), temps.end(), 0);
        return static_cast<int16_t>(sum / temps.size());
    }
    return 0;
  }
  void EventData::SetComputerTemp(const BoardType_e BoardType, const uint8_t index, const int16_t& temp) {
    if (index >= 3) {
        return;
    }
    if (BoardType == BoardType_e::MB) {
        mb_temp_sensors_.update([&](auto& array) {
            array[index] = temp;
        });
    }
    if (BoardType == BoardType_e::EB) {
         eb_temp_sensors_.update([&](auto& array) {
            array[index] = temp;
        });
    }
  }

  uint16_t EventData::GetTemp(const uint8_t index) {
    if (index >= 3) {
        return 0;
    }
    switch (index) {
    case 0:
        return temp_.get().lower_tank;
    case 1:
        return temp_.get().middle_tank;
    case 2:
        return temp_.get().upper_tank;
    default:
        return 0;
    }
  }
  void EventData::SetTemp(const uint8_t index, const int16_t res) {
    if (index >= 3) {
        return;
    }
    temp_.update([&](auto& temp) {
        switch (index) {
        case 0:
            temp.lower_tank = res;
            break;
        case 1:
            temp.middle_tank = res;
            break;
        case 2:
            temp.upper_tank = res;
            break;
        default:
            break;
        }
    });
  }

  uint16_t EventData::GetPress() {
    return press_.get().pressure;
  }
  void EventData::SetPress(uint16_t res) {
    press_.update([&](auto& press) {
        press.pressure = res;
    });
  }

  mavlink_simba_gps_t EventData::GetGPS() {
    return gps_.get();
  }

  void EventData::SetGPS(int32_t lon, int32_t lat, int32_t alt) {
    mavlink_simba_gps_t gps_data;
    gps_data.altitude = alt;
    gps_data.lat = lat;
    gps_data.lon = lon;
    gps_.set(gps_data);
  }

  int32_t EventData::GetMaxAltitude() {
    return max_altitude_.get().altitude;
  }
  void EventData::SetMaxAltitude(const int32_t alt) {
    max_altitude_.update([&](auto& max_alt) {
        max_alt.altitude = std::max(alt, max_alt.altitude);
    });
  }

}  // namespace apps
}  // namespace srp
