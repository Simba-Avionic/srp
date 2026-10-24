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
#include "apps/ec/radio_service/radio_app.h"
#include <utility>
#include <vector>
#include <string>
#include <chrono>  // NOLINT
#include "simba/mavlink.h"
#include "simba/simba.h"
#include "ara/log/log.h"
#include "core/common/condition.h"

namespace srp {
namespace apps {
namespace {
  static constexpr auto kHB_send_time_ms =        990;
  static constexpr auto kMax_altitude_send_time_ms = 1000;
  static constexpr auto kEngine_sensor_send_time_ms = 1000;
  static constexpr auto kGps_send_time_ms = 1000;
  static constexpr auto kComputer_telemetry_send_time_ms = 1000;

  static constexpr auto kRequired_mavlink_messages_to_change_state = 4;

  static constexpr auto kStatic_test_mode = false;
  static constexpr auto kHeartBeatPinID = 2;
  static constexpr auto kHeartbeat_time_ms = 500;

  static constexpr auto kService_ipc_instance =       "srp/apps/RadioApp/RadioService_ipc";
  static constexpr auto kService_udp_instance =       "srp/apps/RadioApp/RadioService_udp";
}  // namespace


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
    // Dump Valve
    update_valve(SIMBA_GS_FLAGS_DUMP_VALVE, SIMBA_ACTUATOR_FLAGS_DUMP_VALVE, "DUMP_VALVE",
                  [&](uint8_t val) { servo_handler->SetOxidizerDumpValve(val); });

    // Vent Valve
    uint8_t requested = ((values & SIMBA_GS_FLAGS_VENT_VALVE) != 0);
    if (servo_handler) {
      ara::log::LogInfo() << "Changing VENT_VALVE to " << (requested ? "ON" : "OFF");
      event_data->SetActuatorState(static_cast<SIMBA_ACTUATOR_FLAGS>(SIMBA_ACTUATOR_FLAGS_VENT_VALVE), requested);
      servo_handler->SetOxidizerVentValve(requested);
    }
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
                      << timestamp << " flags=" << values;

  HBHangleState(values);

  HBHangleActuators(values);
}

void RadioApp::OnRadioStatusMsg(const mavlink_message_t& msg) {
  mavlink_radio_status_t radio_status;
  mavlink_msg_radio_status_decode(&msg, &radio_status);
  ara::log::LogWarn()
      << "rxErrors: "      << radio_status.rxerrors
      << " TxFreeBuf: "    << radio_status.txbuf
      << " Fixed: "        << radio_status.fixed
      << " rssi: "         << radio_status.rssi
      << " remote rssi: "  << radio_status.remrssi
      << " noise: "        << radio_status.noise
      << " remote noise: " << radio_status.remnoise;

  RadioDataType someip_radio_data {
      .rxerrors = radio_status.rxerrors,
      .fixed    = radio_status.fixed,
      .rssi     = radio_status.rssi,
      .remrssi  = radio_status.remrssi,
      .txbuf    = radio_status.txbuf,
      .noise    = radio_status.noise,
      .remnoise = radio_status.remnoise
  };

  service_ipc.RadioStatusEvent.Update(someip_radio_data);
  service_udp.RadioStatusEvent.Update(someip_radio_data);
}

void RadioApp::OnRefuelingCMD(const mavlink_message_t& msg) {
  mavlink_simba_refueling_cmd_t cmd;
  mavlink_msg_simba_refueling_cmd_decode(&msg, &cmd);

  auto fuel_handler = someip_controller.GetFuelServiceHandler();
  if (!fuel_handler) {
      ara::log::LogError() << "Fuel service handler not ready!";
      return;
  }

  if (cmd.command == 1) {  // START
      ara::log::LogInfo() << "Received START REFUELLING command for tank: " << static_cast<int>(cmd.tank_id);

      RefuelRequestType request;
      request.tank_id = cmd.tank_id;
      request.target_value = cmd.target_value;
      fuel_handler->StartRefueling(request);
  } else if (cmd.command == 0) {  // ABORT
      ara::log::LogWarn() << "Received ABORT REFUELLING command for tank: " << static_cast<int>(cmd.tank_id);
      fuel_handler->Abort(cmd.tank_id);
  }
}

void RadioApp::OnRocketMass(const mavlink_message_t& msg) {
  mavlink_simba_rocket_mass_t mass_msg;
  mavlink_msg_simba_rocket_mass_decode(&msg, &mass_msg);

  service_ipc.GSERocketMassEvent.Update(mass_msg.mass);
  service_udp.GSERocketMassEvent.Update(mass_msg.mass);
}

void RadioApp::OnGSOxidizerTankPressure(const mavlink_message_t& msg) {
  mavlink_simba_gs_oxidizer_tank_pressure_t press_msg;
  mavlink_msg_simba_gs_oxidizer_tank_pressure_decode(&msg, &press_msg);

  service_ipc.GSEOxidizerTankPressEvent.Update(press_msg.pressure);
  service_udp.GSEOxidizerTankPressEvent.Update(press_msg.pressure);
}

void RadioApp::OnGSPFSTankPressure(const mavlink_message_t& msg) {
  mavlink_simba_gs_pfs_tank_pressure_t press_msg;
  mavlink_msg_simba_gs_pfs_tank_pressure_decode(&msg, &press_msg);

  service_ipc.GSEPressureFeedSystemTankPressEvent.Update(press_msg.pressure);
  service_udp.GSEPressureFeedSystemTankPressEvent.Update(press_msg.pressure);
}

void RadioApp::RxMsgCallback(const mavlink_message_t& msg) {
  switch (msg.msgid) {
    case MAVLINK_MSG_ID_SIMBA_ACTUATOR_CMD:
      OnActuatorCMD(msg);
      break;
    case MAVLINK_MSG_ID_SIMBA_GS_HEARTBEAT:
      OnGSHEARTBEAT(msg);
      break;
    case MAVLINK_MSG_ID_RADIO_STATUS:
      OnRadioStatusMsg(msg);
      break;
    case MAVLINK_MSG_ID_SIMBA_REFUELING_CMD: {
      OnRefuelingCMD(msg);
      break;
    }
    case MAVLINK_MSG_ID_SIMBA_ROCKET_MASS:
      OnRocketMass(msg);
      break;
    case MAVLINK_MSG_ID_SIMBA_GS_OXIDIZER_TANK_PRESSURE:
      OnGSOxidizerTankPressure(msg);
      break;
    case MAVLINK_MSG_ID_SIMBA_GS_PFS_TANK_PRESSURE:
      OnGSPFSTankPressure(msg);
      break;
    default:
      ara::log::LogInfo() << "Received unknown msg with ID: " <<
          msg.msgid << ", with size: " << msg.len;
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
  timer_ctr_.Start();
  service_ipc.StartOffer();
  service_udp.StartOffer();

  while (!token.stop_requested()) {
    if (gpio_.SetPinValue(kHeartBeatPinID, 1, kHeartbeat_time_ms) != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "EngineApp::Run: Failed to toggle heartbeat pin";
    }
    core::condition::wait_for(std::chrono::milliseconds(kHeartbeat_time_ms * 2), token);
  }
  timer_ctr_.Stop();
  service_ipc.StopOffer();
  service_udp.StopOffer();
  return core::ErrorCode::kOk;
}

int RadioApp::Initialize(const std::map<ara::core::StringView,
  ara::core::StringView> parms) {
  event_data = EventData::GetInstance();
  timer_ctr_.AddOnTimerCallback([this](){
    auto hb = telemetry_provider.GetHeartbeatMsg();
    if (hb.has_value()) {
      radio_controller.Push(hb.value());
    }
  }, 5000);

  timer_ctr_.AddOnTimerCallback([this](){
    // Max Altitude
    radio_controller.Push(telemetry_provider.GetMaxAltitudeMsg());
  }, 5000);

  // timer_ctr_.AddOnTimerCallback([this](){
  //   // Tank Sensors
  //   radio_controller.Push(telemetry_provider.GetTankSensorsMsg());
  // }, kEngine_sensor_send_time_ms);

  timer_ctr_.AddOnTimerCallback([this](){
    // GPS
    radio_controller.Push(telemetry_provider.GetGpsMsg());
  }, 2000);

  // timer_ctr_.AddOnTimerCallback([this](){
  //   // Computers telemetry
  //   radio_controller.Push(telemetry_provider.GetComputersTelemetryMsg());
  // }, kComputer_telemetry_send_time_ms);

  timer_ctr_.AddOnTimerCallback([this](){
    radio_controller.Push(telemetry_provider.GetRefuelingStatusMsg());
  }, 1000);

  someip_controller.SetRefuelStateChangedCallback([this]() {
      auto msg = telemetry_provider.GetRefuelingStatusMsg();
      radio_controller.Push(msg);
  });

  someip_controller.SetValveCmdCallback([this](uint8_t valve_id, uint8_t pos) {
      auto msg = telemetry_provider.GetGSValveCmdMsg(valve_id, pos);
      radio_controller.Push(msg);
      ara::log::LogInfo() << "Valve command send to ground segment for valve with id: " << static_cast<int>(valve_id);
  });

  return core::ErrorCode::kOk;
}

RadioApp::RadioApp():
          service_ipc(ara::core::InstanceSpecifier{kService_ipc_instance}),
          service_udp(ara::core::InstanceSpecifier{kService_udp_instance}) {
}

}  // namespace apps
}  // namespace srp
