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
#include <termios.h>
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
  static constexpr auto KRadio_UART_path =     "/dev/ttyS1";
  static constexpr auto KRadio_UART_baudrate = B57600;
  static constexpr auto kTime =        990;
  static constexpr auto kRequired_mavlink_messages_to_change_state = 4;

  static constexpr auto kEC_enabled =       true;
  static constexpr auto kFC_enabled =       true;
  static constexpr auto kStatic_test_mode = false;
  static constexpr auto kHeartBeatPinID = 2;

  static constexpr auto kHbDelayWarning = 1.1;

  static constexpr auto kDuration_from_last_hb_to_conn_lost_ms = 2 * 60 * 1000;
  static constexpr auto kDuration_from_last_hb_to_abort_ms = 15 * 60 * 1000;

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
      UartTxQueue.Push(hb.value());
    }

    // Max Altitude
    UartTxQueue.Push(telemetry_provider.GetMaxAltitudeMsg());

    // Tank Sensors
    UartTxQueue.Push(telemetry_provider.GetTankSensorsMsg());

    // GPS
    UartTxQueue.Push(telemetry_provider.GetGpsMsg());

    // Computers telemetry
    UartTxQueue.Push(telemetry_provider.GetComputersTelemetryMsg());

    WaitUntillTimeEnd(start, kTime, token);
  }
}
std::optional<RocketState_t> RadioApp::GetReqRocketStateFromGSFlags(const uint8_t flags) {
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
        mavl_logger.LogWarn() << "Servo service handler not ready for " << name;
        return;
      }
      mavl_logger.LogInfo() << "Changing " << name << " to " << (requested ? "ON" : "OFF");
      event_data->SetActuatorState(static_cast<SIMBA_ACTUATOR_FLAGS>(rocket_mask), requested);
      setter_func(requested);
    }
  };

  auto eb_state = event_data->GetComputerState(BoardType_e::EB);

  if (eb_state == RocketState_t::ARM || (kStatic_test_mode && eb_state != RocketState_t::DISARM)) {
    update_valve(SIMBA_GS_FLAGS_VENT_VALVE, SIMBA_ACTUATOR_FLAGS_VENT_VALVE, "VENT_VALVE",
                  [&](uint8_t val) { servo_handler->SetVentServoValue(val); });
    update_valve(SIMBA_GS_FLAGS_DUMP_VALVE, SIMBA_ACTUATOR_FLAGS_DUMP_VALVE, "DUMP_VALVE",
                  [&](uint8_t val) { servo_handler->SetDumpValue(val); });
  }

  if (event_data->GetComputerState(BoardType_e::MB) == RocketState_t::ARM) {
    // TODO(matikrajek42@gmail.com) Add missing Cameras handler
  }
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
  auto req_state = GetReqRocketStateFromGSFlags(values);
  if (!req_state.has_value()) {
    return;
  }
  auto current_mb = event_data->GetComputerState(BoardType_e::MB);
  auto current_eb = event_data->GetComputerState(BoardType_e::EB);

  if (!(((req_state.value() != current_eb) && kEC_enabled) || ((req_state.value() != current_mb) && kFC_enabled))) {
    return;
  }

  if (!IsStateChangeApproved(req_state.value())) {
    return;
  }
  auto engine_handler = someip_controller.GetEngineServiceHandler();
  auto main_handler = someip_controller.GetMainServiceHandler();


  if ((engine_handler == nullptr)) {
    mavl_logger.LogWarn() << "Service handlers not ready in GS_HEARTBEAT (engine).";
  } else {
    engine_handler->SetMode(static_cast<uint8_t>(req_state.value()));
  }
  if ((main_handler == nullptr)) {
    mavl_logger.LogWarn() << "Service handlers not ready in GS_HEARTBEAT (main).";
  } else {
    main_handler->setMode(static_cast<uint8_t>(req_state.value()));
  }
  mavl_logger.LogInfo() << "Changing rocket state to "
                    + core::rocketState::to_string(req_state.value());
}

void RadioApp::OnGSHEARTBEAT(const mavlink_message_t& msg) {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_received_hb).count();
  if (duration > (1000 * kHbDelayWarning)) {
    ara::log::LogWarn() << "GS Heartbeat received with duration: " << std::to_string(duration);
  } else {
    ara::log::LogDebug() << "GS Heartbeat received with duration: " << std::to_string(duration);
  }

  last_received_hb = now;

  auto timestamp = mavlink_msg_simba_gs_heartbeat_get_timestamp(&msg);
  auto values = mavlink_msg_simba_gs_heartbeat_get_values(&msg);
  mavl_logger.LogInfo() << "GS Heartbeat: ts="
                      + std::to_string(static_cast<int64_t>(timestamp))
                      + " flags=" + std::to_string(static_cast<int>(values));

  HBHangleState(values);

  HBHangleActuators(values);
}

void RadioApp::ListeningLoop(const std::stop_token& token) {
  mavlink_message_t msg;
  mavlink_status_t status;
  while (!token.stop_requested()) {
    if (!event_data) {
      mavl_logger.LogError() << "EventData not initialized in ListeningLoop. Stopping loop.";
      return;
    }
    if (!uart_.WaitForData(250)) {
      continue;
    }
    auto bytes_opt = uart_.Read(0);
    if (!bytes_opt.has_value()) {
      continue;
    }
    for (uint8_t i = 0; i < bytes_opt.value().size(); i++) {
      if (!mavlink_parse_char(MAVLINK_COMM_0, bytes_opt.value()[i], &msg, &status)) {
        continue;
      }
      mavl_logger.LogDebug() << "Parsed MAVLink msg id="
                          + std::to_string(msg.msgid)
                          + " len=" + std::to_string(msg.len);
      switch (msg.msgid) {
        case MAVLINK_MSG_ID_SIMBA_ACTUATOR_CMD:
          OnActuatorCMD(msg);
          break;
        case MAVLINK_MSG_ID_SIMBA_GS_HEARTBEAT:
          OnGSHEARTBEAT(msg);
          break;
        default:
          mavl_logger.LogInfo() << "Received unknown msg with ID: " <<
              std::to_string(msg.msgid) << ", with size: " << std::to_string(msg.len);
          break;
      }
    }
  }
}

int RadioApp::Run(const std::stop_token& token) {
  if (!event_data) {
    ara::log::LogWarn() << "EventData not initialized in Run. Creating instance.";
    event_data = EventData::GetInstance();
  }
  ara::log::LogDebug() << "RadioApp Run starting threads";
  std::jthread uart_transmit_thread([this](const std::stop_token& t) {
    while (!t.stop_requested()) {
      auto package_opt = UartTxQueue.Get(t);
      if (!package_opt.has_value()) {
        continue;
      }
      uart_.Write(package_opt.value());
    }
  });
  std::jthread transmitting_thread([this](const std::stop_token& t) {
    this->TransmittingLoop(t);
    });
  std::jthread listening_thread([this](const std::stop_token& t) {
    this->ListeningLoop(t);
  });
  std::jthread gs_communication_guard_thread([this](const std::stop_token& t) {
    auto broadcast_state = [this](RocketState_t state, const std::string& reason) {
      ara::log::LogError() << "GS Connection Guard: " << reason << " -> Switching to " 
                            << core::rocketState::to_string(state);
      
      auto raw_state = static_cast<uint8_t>(state);
      auto engine_handler = someip_controller.GetEngineServiceHandler();
      auto main_handler = someip_controller.GetMainServiceHandler();
      if (engine_handler) engine_handler->SetMode(raw_state);
      if (main_handler)   main_handler->setMode(raw_state);
    };
    while (!t.stop_requested()) {
      auto now = std::chrono::high_resolution_clock::now();
      auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_received_hb).count();

      if (diff >= kDuration_from_last_hb_to_abort_ms) {
          broadcast_state(RocketState_t::ABORT, "Timeout ABORT");
      } 
      else if (diff >= kDuration_from_last_hb_to_conn_lost_ms) {
          broadcast_state(RocketState_t::CONNECTION_LOST, "Timeout CONN_LOST");
      }
      core::condition::wait_for(std::chrono::milliseconds(100), t);
    }
  });

  while (!token.stop_requested()) {
    if (gpio_.SetPinValue(kHeartBeatPinID, 1, 500) != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "EngineApp::Run: Failed to toggle heartbeat pin";
    }
  }

  ara::log::LogDebug() << "RadioApp Run stopping services and UART";
  service_ipc->StopOffer();
  service_udp->StopOffer();
  uart_.Close();
  return core::ErrorCode::kOk;
}

int RadioApp::Initialize(const std::map<ara::core::StringView,
   ara::core::StringView> parms) {
    ara::log::LogDebug() << "RadioApp Initialize called";
    event_data = EventData::GetInstance();
    ara::log::LogDebug() << "EventData instance created in Initialize";
    if (!this->uart_.Open(KRadio_UART_path, KRadio_UART_baudrate, 10)) {
        ara::log::LogError() << "Failed to open UART: " <<  KRadio_UART_path;
        return 1;
    }
    service_ipc = std::make_unique<apps::RadioServiceSkeleton>(service_ipc_instance);
    service_udp = std::make_unique<apps::RadioServiceSkeleton>(service_udp_instance);
    service_ipc->StartOffer();
    service_udp->StartOffer();
    ara::log::LogDebug() << "RadioServiceSkeleton offers started (IPC & UDP)";
    return core::ErrorCode::kOk;
}

RadioApp::~RadioApp() {}

RadioApp::RadioApp():
          mavl_logger{ara::log::LoggingMenager::GetInstance()->CreateLogger("MAVL", "", ara::log::LogLevel::kDebug)},
          service_ipc_instance(kService_ipc_instance),
          service_udp_instance(kService_udp_instance) {
}

}  // namespace apps
}  // namespace srp
