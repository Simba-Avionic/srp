/**
 * @file radio_app.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "apps/fc/radio_service/radio_app.h"
#include <utility>
#include <vector>
#include <string>
#include <chrono>  // NOLINT
#include "simba/mavlink.h"
#include "simba/simba.h"
#include "ara/log/log.h"
#include "core/common/condition.h"
#include "apps/ec/engine_service/vent_controller.hpp"

namespace srp {
namespace apps {
namespace {
  static constexpr auto kTime =        990;
  static constexpr auto kRequired_mavlink_messages_to_change_state = 4;

  static constexpr auto kStatic_test_mode = false;
  static constexpr auto kHeartBeatPinID = 2;
  static constexpr auto kHeartbeat_time_ms = 500;

  static constexpr auto kService_ipc_instance =       "srp/apps/RadioApp/RadioService_ipc";
  static constexpr auto kService_udp_instance =       "srp/apps/RadioApp/RadioService_udp";
}  // namespace



void RadioApp::WaitUntillTimeEnd(const timepoint& start, const uint32_t req_loop_time,
                                                  const std::stop_token& token) {
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  if (std::chrono::milliseconds(req_loop_time) > duration) {
      core::condition::wait_for(std::chrono::milliseconds(req_loop_time) - duration, token);
  }
}

void RadioApp::TransmittingLoop(const std::stop_token& token) {
  while (!token.stop_requested()) {
    auto start = std::chrono::high_resolution_clock::now();

    // HB
    auto hb = telemetry_provider.GetHeartbeatMsg();
    if (hb.has_value()) {
      radio_controller.Push(hb.value());
    }

    // Max Altitude
    radio_controller.Push(telemetry_provider.GetMaxAltitudeMsg());

    // Tank Sensors
    radio_controller.Push(telemetry_provider.GetTankSensorsMsg());

    // GPS
    radio_controller.Push(telemetry_provider.GetGpsMsg());

    // Computers telemetry
    radio_controller.Push(telemetry_provider.GetComputersTelemetryMsg());

    WaitUntillTimeEnd(start, kTime, token);
  }
}

void RadioApp::OnActuatorCMD(const mavlink_message_t& msg) {
  // TODO(matikrajek42@gmail.com) IMplement this maybe
}

void RadioApp::HBHangleActuators(const uint8_t values) {
  std::shared_ptr<ServoServiceHandler> servo_handler = someip_controller.GetServoServiceHandler();
  auto update_valve = [&](uint8_t gs_mask, uint8_t rocket_mask, const std::string& name, auto setter_func) {
    uint8_t requested = ((values & gs_mask) != 0);
    bool current = (event_data->GetActuatorStates() & rocket_mask) != 0;

    if (requested != current) {
      if (!servo_handler) {
        ara::log::LogWarn() << "Servo service handler not ready for " << name;
        return;
      }
      ara::log::LogInfo() << "Changing " << name << " to " << (requested ? "ON" : "OFF");
      event_data->SetActuatorState(static_cast<SIMBA_ACTUATOR_FLAGS>(rocket_mask), requested);
      setter_func(requested);
    }
  };

  auto eb_state = event_data->GetComputerState(BoardType_e::EB);

  if (eb_state == RocketState_t::ARM || (kStatic_test_mode && eb_state == RocketState_t::DISARM)) {
    update_valve(SIMBA_GS_FLAGS_VENT_VALVE, SIMBA_ACTUATOR_FLAGS_VENT_VALVE, "VENT_VALVE",
                  [&](uint8_t val) {
      auto to_set = (val == 0) ? engine::VentState_e::CLOSE : engine::VentState_e::OPENING;
      auto engine_handler = someip_controller.GetEngineServiceHandler();
      engine_handler->SetVentValve(static_cast<uint8_t>(to_set));
    });
    update_valve(SIMBA_GS_FLAGS_DUMP_VALVE, SIMBA_ACTUATOR_FLAGS_DUMP_VALVE, "DUMP_VALVE",
                  [&](uint8_t val) { servo_handler->SetDumpValue(val); });
  }

  // TODO(matikrajek42@gmail.com) Add missing Cameras handler
}

/**
 * @brief Spełnienie wymagania konkursu FAROUT -> pojedyńcza wiadomość mavlink nie może wywołać zmiany stanu
 * 
 * @param req_state 
 * @return true 
 * @return false 
 */
bool RadioApp::IsStateChangeApproved(const RocketState_t req_state) {
  if (req_state != req_rocket_state.first) {
    req_rocket_state.first = req_state;
    req_rocket_state.second = 1;
    return false;
  }
  req_rocket_state.second += 1;
  if (req_rocket_state.second >= kRequired_mavlink_messages_to_change_state) {
    return true;
  }
  return false;
}

void RadioApp::HBHangleState(const uint8_t values) {
  auto req_state = telemetry_provider.GetReqRocketStateFromGSFlags(values);
  if (!req_state.has_value()) {
    return;
  }
  auto current_mb = event_data->GetComputerState(BoardType_e::MB);
  auto current_eb = event_data->GetComputerState(BoardType_e::EB);

  auto engine_handler = someip_controller.GetEngineServiceHandler();
  auto main_handler = someip_controller.GetMainServiceHandler();
  if (!((engine_handler && (req_state.value() != current_eb)) ||
            ((main_handler && (req_state.value() != current_mb))))) {
    return;
  }

  if (!IsStateChangeApproved(req_state.value())) {
    return;
  }

  if (engine_handler) {
    engine_handler->SetMode(static_cast<uint8_t>(req_state.value()));
  } else {
    ara::log::LogWarn() << "Service handlers not ready in GS_HEARTBEAT (engine).";
  }
  if (main_handler) {
    main_handler->setMode(static_cast<uint8_t>(req_state.value()));
  } else {
    ara::log::LogWarn() << "Service handlers not ready in GS_HEARTBEAT (main).";
  }
  ara::log::LogInfo() << "Changing rocket state to "
                    + core::rocketState::to_string(req_state.value());
}

void RadioApp::OnGSHEARTBEAT(const mavlink_message_t& msg) {
  heartbeat_controller.OnNewHbCallback();

  auto timestamp = mavlink_msg_simba_gs_heartbeat_get_timestamp(&msg);
  auto values = mavlink_msg_simba_gs_heartbeat_get_values(&msg);
  ara::log::LogInfo() << "GS Heartbeat: ts="
                      + std::to_string(static_cast<int64_t>(timestamp))
                      + " flags=" + std::to_string(static_cast<int>(values));

  HBHangleState(values);

  HBHangleActuators(values);
}

void RadioApp::RxMsgCallback(const mavlink_message_t& msg) {
  switch (msg.msgid) {
    case MAVLINK_MSG_ID_SIMBA_ACTUATOR_CMD:
      OnActuatorCMD(msg);
      break;
    case MAVLINK_MSG_ID_SIMBA_GS_HEARTBEAT:
      OnGSHEARTBEAT(msg);
      break;
    default:
      ara::log::LogInfo() << "Received unknown msg with ID: " <<
          std::to_string(msg.msgid) << ", with size: " << std::to_string(msg.len);
      break;
  }
}

int RadioApp::Run(const std::stop_token& token) {
  radio_controller.Init([this](const mavlink_message_t& msg) { RxMsgCallback(msg); }, token);
  heartbeat_controller.Init([this](const RocketState_t state) {
    auto engine_handler = someip_controller.GetEngineServiceHandler();
    auto main_handler = someip_controller.GetMainServiceHandler();
    if (engine_handler) {
      engine_handler->SetMode(static_cast<uint8_t>(state));
    }
    if (main_handler) {
      main_handler->setMode(static_cast<uint8_t>(state));
    }
  });
  ara::log::LogDebug() << "RadioApp Run starting threads";

  std::jthread transmitting_thread([this](const std::stop_token& t) {
    this->TransmittingLoop(t);
  });

  std::jthread gs_communication_guard_thread([this](const std::stop_token& t) {
    GSHeartbeatGuardLoop(t);
  });
  service_ipc.StartOffer();
  service_udp.StartOffer();

  while (!token.stop_requested()) {
    if (gpio_.SetPinValue(kHeartBeatPinID, 1, kHeartbeat_time_ms) != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "EngineApp::Run: Failed to toggle heartbeat pin";
    }
    core::condition::wait_for(std::chrono::milliseconds(kHeartbeat_time_ms * 2), token);
  }

  service_ipc.StopOffer();
  service_udp.StopOffer();
  return core::ErrorCode::kOk;
}

int RadioApp::Initialize(const std::map<ara::core::StringView,
  ara::core::StringView> parms) {
  event_data = EventData::GetInstance();
  return core::ErrorCode::kOk;
}

RadioApp::RadioApp():
          service_ipc(ara::core::InstanceSpecifier{kService_ipc_instance}),
          service_udp(ara::core::InstanceSpecifier{kService_udp_instance}) {
}

}  // namespace apps
}  // namespace srp
