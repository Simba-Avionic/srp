/**
 * @file telemetry_provider.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "apps/fc/radio_service/telemetry_provider.hpp"
#include <utility>
#include "ara/log/log.h"
namespace srp {
namespace apps {
namespace radio {

namespace {
    static constexpr auto kSystemId = 1;
    static constexpr auto kComponentId = 200;

    uint8_t RocketStateToMavlinkState(const core::rocketState::RocketState_t state) {
        switch (state) {
            using RocketState_t = core::rocketState::RocketState_t;
            case RocketState_t::INIT:             return SIMBA_ROCKET_STATE_INIT;
            case RocketState_t::DISARM:           return SIMBA_ROCKET_STATE_DISARM;
            case RocketState_t::ARM:              return SIMBA_ROCKET_STATE_ARM;
            case RocketState_t::LAUNCH:           return SIMBA_ROCKET_STATE_LAUNCH;
            case RocketState_t::FLIGHT:           return SIMBA_ROCKET_STATE_FLIGHT;
            case RocketState_t::APOGEE:           return SIMBA_ROCKET_STATE_APOGEE;
            case RocketState_t::FIRST_PARACHUTE:  return SIMBA_ROCKET_STATE_DESCENT_PILOT;
            case RocketState_t::SECOND_PARACHUTE: return SIMBA_ROCKET_STATE_SEC_PARACHUTE;
            case RocketState_t::DROP:             return SIMBA_ROCKET_STATE_DESCENT_MAIN;
            case RocketState_t::ABORT:            return SIMBA_ROCKET_STATE_ABORT;
            case RocketState_t::CONNECTION_LOST:  return SIMBA_ROCKET_STATE_CONNECTION_LOST;
            case RocketState_t::TOUCHDOWN:        return SIMBA_ROCKET_STATE_TOUCHDOWN;
            default:                              return SIMBA_ROCKET_STATE_ABORT;
        }
    }

    uint8_t PrimerStateToMavlinState(const srp::primer::PrimerState_t state) {
        switch (state) {
            using PrimerState_t = srp::primer::PrimerState_t;
            case PrimerState_t::kUNKNOWN:       return SIMBA_PRIMER_STATE_UNKNOWN;
            case PrimerState_t::kCONNECTED:     return SIMBA_PRIMER_STATE_CONNECTED;
            case PrimerState_t::kNOT_CONNECTED: return SIMBA_PRIMER_STATE_NOT_CONNECTED;
            case PrimerState_t::kSHORT_CIRCUIT: return SIMBA_PRIMER_STATE_SHORT_CIRCUIT;
            default:                            return SIMBA_PRIMER_STATE_UNKNOWN;
        }
    }
}  // namespace

std::optional<RocketState_t> TelemetryProvider::GetReqRocketStateFromGSFlags(const uint8_t flags) {
  static constexpr std::pair<uint8_t, RocketState_t> gs_rocket_state_mapping[] = {
      {SIMBA_GS_FLAGS_ABORT,  RocketState_t::ABORT},
      {SIMBA_GS_FLAGS_LAUNCH,  RocketState_t::LAUNCH},
      {SIMBA_GS_FLAGS_ARM,     RocketState_t::ARM},
      {SIMBA_GS_FLAGS_DISARM,  RocketState_t::DISARM},
    };
    for (const auto& [mask, state] : gs_rocket_state_mapping) {
        if ((flags & mask) != 0) return state;
    }

    return std::nullopt;
}

TelemetryProvider::TelemetryProvider() : event_data(EventData::GetInstance()) {
    if (!timestamp_.Init()) {
        ara::log::LogWarn() << "TelemetryProvider: Cant initialize app time";
    }
}

uint8_t TelemetryProvider::MapState(core::rocketState::RocketState_t s) {
    return RocketStateToMavlinkState(s);
}

uint8_t TelemetryProvider::MapPrimer(srp::primer::PrimerState_t p) {
    return PrimerStateToMavlinState(p);
}

std::optional<std::vector<uint8_t>> TelemetryProvider::GetHeartbeatMsg() {
    auto ts = timestamp_.GetNewTimeStamp();
    if (!ts.has_value()) return std::nullopt;
    return Pack([&](mavlink_message_t* m) {
      mavlink_msg_simba_rocket_heartbeat_pack(
          kSystemId, kComponentId, m,
          static_cast<uint64_t>(ts.value()), 0,
          MapState(event_data->GetComputerState(BoardType_e::MB)),
          MapState(event_data->GetComputerState(BoardType_e::EB)),
          event_data->GetActuatorStates(),
          MapPrimer(static_cast<srp::primer::PrimerState_t>(event_data->GetPrimerStates())));
    });
}

std::vector<uint8_t> TelemetryProvider::GetMaxAltitudeMsg() {
    return Pack([&](mavlink_message_t* m) {
      mavlink_msg_simba_max_altitude_pack(kSystemId, kComponentId, m, event_data->GetMaxAltitude());
    });
}

std::vector<uint8_t> TelemetryProvider::GetTankSensorsMsg() {
    return Pack([&](mavlink_message_t* m) {
        mavlink_msg_simba_tank_sensors_pack(kSystemId, kComponentId, m,
            static_cast<int16_t>(event_data->GetTemp(0)),
            static_cast<int16_t>(event_data->GetTemp(1)),
            static_cast<int16_t>(event_data->GetTemp(2)),
            event_data->GetPress());
    });
}

std::vector<uint8_t> TelemetryProvider::GetGpsMsg() {
    auto gps_data = event_data->GetGPS();
    return Pack([&](mavlink_message_t* m) {
      mavlink_msg_simba_gps_pack(kSystemId, kComponentId, m, gps_data.lon, gps_data.lat, gps_data.altitude);
    });
}

std::vector<uint8_t> TelemetryProvider::GetComputersTelemetryMsg() {
    auto eb = event_data->GetComputerTelemetry(BoardType_e::EB);
    auto mb = event_data->GetComputerTelemetry(BoardType_e::MB);

    return Pack([&](mavlink_message_t* m) {
      mavlink_msg_simba_computers_telemetry_pack(
          kSystemId, kComponentId, m,
          mb.cpu_usage, mb.mem_usage,
          eb.cpu_usage, eb.mem_usage,
          mb.temps, eb.temps);
    });
}

}  // namespace radio
}  // namespace apps
}  // namespace srp
