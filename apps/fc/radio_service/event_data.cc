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

uint16_t EventData::GetComputerAppsAlive(const BoardType_e BoardType) {
  switch (BoardType) {
  case BoardType_e::EB:
    return eb_apps_alive.get();
    break;
  case BoardType_e::MB:
    return mb_apps_alive.get();
  default:
    return 0;
  }
}

void EventData::SetComputerAppsAlive(const BoardType_e BoardType, const uint16_t apps_alive) {
  switch (BoardType)
  {
  case BoardType_e::EB:
    eb_apps_alive.set(apps_alive);
    break;
  case BoardType_e::MB:
    mb_apps_alive.set(apps_alive);
    break;
  default:
    break;
  }
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

computer_telemetry_t EventData::GetComputerTelemetry(const BoardType_e BoardType) {
  if (BoardType == BoardType_e::EB) {
    return eb_telemetry.get();
  } else {
    return mb_telemetry.get();
  }
}
void EventData::SetComputerTemp(const BoardType_e BoardType, const uint8_t temp, const uint8_t index) {
  if (index >= 3) {
      return;
  }
  if (BoardType == BoardType_e::EB) {
    eb_telemetry.update([&](auto& telemetry) {
      telemetry.temps[index] = temp;
    });
  } else if (BoardType == BoardType_e::MB) {
    mb_telemetry.update([&](auto& telemetry) {
      telemetry.temps[index] = temp;
    });
  }
}
void EventData::SetComputerCpuUsage(const BoardType_e BoardType, const uint8_t cpu_usage) {
  if (BoardType == BoardType_e::EB) {
    eb_telemetry.update([&](auto& telemetry) {
      telemetry.cpu_usage = cpu_usage;
    });
  } else if (BoardType == BoardType_e::MB) {
    mb_telemetry.update([&](auto& telemetry) {
      telemetry.cpu_usage = cpu_usage;
    });
  }
}
void EventData::SetComputerMemUsage(const BoardType_e BoardType, const uint8_t mem_usage) {
  if (BoardType == BoardType_e::EB) {
    eb_telemetry.update([&](auto& telemetry) {
      telemetry.mem_usage = mem_usage;
    });
  } else if (BoardType == BoardType_e::MB) {
    mb_telemetry.update([&](auto& telemetry) {
      telemetry.mem_usage = mem_usage;
    });
  }
}

uint16_t EventData::GetTemp(const uint8_t index) {
  if (index >= 3) {
      return 0;
  }
  switch (index) {
  case 0:
      return tank_.get().lower_tank;
  case 1:
      return tank_.get().middle_tank;
  case 2:
      return tank_.get().upper_tank;
  default:
      return 0;
  }
}
void EventData::SetTemp(const uint8_t index, const int16_t res) {
  if (index >= 3) {
      return;
  }
  tank_.update([&](auto& temp) {
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
  return tank_.get().pressure;
}
void EventData::SetPress(uint16_t res) {
  tank_.update([&](auto& tank) {
    tank.pressure = res;
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
