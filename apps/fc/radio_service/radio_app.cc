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
using RocketState_t = core::rocketState::RocketState_t;
namespace {
  static constexpr auto KRadio_UART_path = "/dev/ttyS1";
  static constexpr auto KRadio_UART_baudrate = B57600;
  static constexpr auto kSystemId = 1;
  static constexpr auto kComponentId = 200;
  static constexpr auto kTime = 990;  // Should be 1 Hz but better make it 1.1Hz than 0.9 wchich can trigger error on GS
  static constexpr auto kRequired_mavlink_messages_to_change_state = 5;

  static constexpr auto kEC_enabled = true;
  static constexpr auto kFC_enabled = false;
  static constexpr auto kStatic_test_mode = true;
}  // namespace

namespace {
  static constexpr auto kService_ipc_instance = "srp/apps/RadioApp/RadioService_ipc";
  static constexpr auto kService_udp_instance = "srp/apps/RadioApp/RadioService_udp";
  static constexpr auto kEnv_service_path_name = "srp/apps/RadioApp/EnvApp";
  static constexpr auto kGPS_service_path_name = "srp/apps/RadioApp/GPSService";
  static constexpr auto kPrimer_service_path_name = "srp/apps/RadioApp/PrimerService";
  static constexpr auto kServo_service_path_name = "srp/apps/RadioApp/ServoService";
  static constexpr auto kRecovery_service_path_name = "srp/apps/RadioApp/RecoveryService";
  static constexpr auto kMain_service_path_name = "srp/apps/RadioApp/MainService";
  static constexpr auto kEngine_service_path_name = "srp/apps/RadioApp/EngineService";
}  // namespace

uint8_t RadioApp::RocketStateToMavlinkState(const RocketState_t state) const noexcept {
  switch (state) {
      case RocketState_t::INIT:
          return SIMBA_ROCKET_STATE_INIT;
      case RocketState_t::DISARM:
          return SIMBA_ROCKET_STATE_DISARM;
      case RocketState_t::ARM:
          return SIMBA_ROCKET_STATE_ARM;
      case RocketState_t::LAUNCH:
          return SIMBA_ROCKET_STATE_LAUNCH;
      case RocketState_t::FLIGHT:
          return SIMBA_ROCKET_STATE_FLIGHT;
      case RocketState_t::APOGEE:
          return SIMBA_ROCKET_STATE_APOGEE;
      case RocketState_t::FIRST_PARACHUTE:
          return SIMBA_ROCKET_STATE_FIRT_PARACHUTE_FALL;
      case RocketState_t::SECOND_PARACHUTE:
          return SIMBA_ROCKET_STATE_SECOND_PARACHUTE_ACTIVATION;
      case RocketState_t::DROP:
          return SIMBA_ROCKET_STATE_SECOND_PARACHUTE_FALL;
      case RocketState_t::ABORT:
          return SIMBA_ROCKET_STATE_SIMBA_ROCKET_STATE_ABORT;
      default:
          return SIMBA_ROCKET_STATE_SIMBA_ROCKET_STATE_ABORT;
  }
}

void RadioApp::TransmittingLoop(const std::stop_token& token) {
  mavlink_message_t msg;
  std::vector<uint8_t> buffer(MAVLINK_MAX_PACKET_LEN);

  if (!timestamp_.Init()) {
    ara::log::LogWarn() << "Cant initialize app time";
  }

  event_data = EventData::GetInstance();

  auto send = [&](auto pack_func) {
    pack_func();

    uint16_t len = mavlink_msg_to_send_buffer(buffer.data(), &msg);

    std::vector<uint8_t> actual_data(buffer.begin(), buffer.begin() + len);
    UartTxQueue.Push(actual_data);
  };

  while (!token.stop_requested()) {
    auto start = std::chrono::high_resolution_clock::now();

    // Heartbeat
    if (auto val = timestamp_.GetNewTimeStamp(); val.has_value()) {
      send([&] {
        mavlink_msg_simba_rocket_heartbeat_pack(kSystemId, kComponentId, &msg,
                  static_cast<uint64_t>(val.value()),
                  RocketStateToMavlinkState(event_data->GetComputerState(BoardType_e::MB)),
                  RocketStateToMavlinkState(event_data->GetComputerState(BoardType_e::EB)),
                  event_data->GetActuatorStates());
      });
    }

    // Temperature
    send([&] {
      mavlink_msg_simba_computer_temperature_pack(kSystemId, kComponentId, &msg,
          event_data->GetComputerTemp(BoardType_e::EB), event_data->GetComputerTemp(BoardType_e::MB));
    });

    // Max Altitude
    send([&] {
      mavlink_msg_simba_max_altitude_pack(kSystemId, kComponentId, &msg, event_data->GetMaxAltitude());
    });

    // Tank Temps
    send([&] {
      mavlink_msg_simba_tank_temperature_pack(kSystemId, kComponentId, &msg,
          static_cast<int16_t>(event_data->GetTemp(0)),
          static_cast<int16_t>(event_data->GetTemp(1)),
          static_cast<int16_t>(event_data->GetTemp(2)));
    });

    // Tank Pressure
    send([&] {
      mavlink_msg_simba_tank_pressure_pack(kSystemId, kComponentId, &msg,
          static_cast<uint16_t>(event_data->GetPress()));
    });

    // GPS
    auto gps_data = event_data->GetGPS();
    send([&] {
      mavlink_msg_simba_gps_pack(kSystemId, kComponentId, &msg,
          gps_data.lon, gps_data.lat, gps_data.altitude);
    });

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (std::chrono::milliseconds(kTime) > duration) {
        core::condition::wait_for(std::chrono::milliseconds(kTime) - duration, token);
    }
  }
}
std::optional<RocketState_t> RadioApp::GetReqRocketStateFromGSFlags(const uint8_t flags) {
  static constexpr std::pair<uint8_t, RocketState_t> gs_rocket_state_mapping[] = {
      {SIMBA_GS_ABORT,  RocketState_t::ABORT},
      {SIMBA_GS_LAUNCH, RocketState_t::LAUNCH},
      {SIMBA_GS_ARM,    RocketState_t::ARM},
      {SIMBA_GS_DISARM, RocketState_t::DISARM},
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
  auto update_valve = [&](uint8_t gs_mask, uint8_t rocket_mask, const std::string& name, auto setter_func) {
    uint8_t requested = ((values & gs_mask) != 0);
    bool current = (event_data->GetActuatorStates() & rocket_mask) != 0;

    if (requested != current) {
      if (!servo_service_handler) {
        mavl_logger.LogWarn() << "Servo service handler not ready for " << name;
        return;
      }
      mavl_logger.LogInfo() << "Changing " << name << " to " << (requested ? "ON" : "OFF");
      event_data->SetActuatorState(static_cast<SIMBA_ACTUATOR_FLAGS>(rocket_mask), requested);
      setter_func(requested);
    }
  };

  // TOFIX OBEJSCIE NA STATIC PO KONKURSIE USUNAC FLIGHT JAKO DOZWOLONY
  auto eb_state = event_data->GetComputerState(BoardType_e::EB);
  if (eb_state == RocketState_t::ARM || (kStatic_test_mode && eb_state != RocketState_t::DISARM)) {
    update_valve(SIMBA_GS_VENT_VALVE, SIMBA_ROCKET_VENT_VALVE, "VENT_VALVE",
                  [&](uint8_t val) { servo_service_handler->SetVentServoValue(val); });
    update_valve(SIMBA_GS_DUMP_VALVE, SIMBA_ROCKET_DUMP_VALVE, "DUMP_VALVE",
                  [&](uint8_t val) { servo_service_handler->SetDumpValue(val); });
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
  if ((engine_service_handler == nullptr) && kEC_enabled || (main_service_handler == nullptr) && kFC_enabled) {
    mavl_logger.LogWarn() << "Service handlers not ready in GS_HEARTBEAT (main/engine).";
    return;
  }
  mavl_logger.LogInfo() << "Changing rocket state to "
                    + core::rocketState::to_string(req_state.value());
  if (kFC_enabled) {
    main_service_handler->setMode(static_cast<uint8_t>(req_state.value()));
  }
  if (kEC_enabled) {
    engine_service_handler->SetMode(static_cast<uint8_t>(req_state.value()));
  }
}

void RadioApp::OnGSHEARTBEAT(const mavlink_message_t& msg) {
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
  std::jthread transmitting_thread([this](const std::stop_token& t) {
    this->TransmittingLoop(t);
    });
  std::jthread listening_thread([this](const std::stop_token& t) {
    this->ListeningLoop(t);
  });

  while (!token.stop_requested()) {
    auto package_opt = UartTxQueue.Get(token);
    if (!package_opt.has_value()) {
      continue;
    }
    uart_.Write(package_opt.value());
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
    this->SomeIpInit();
    return core::ErrorCode::kOk;
}

RadioApp::~RadioApp() {
}

RadioApp::RadioApp():
          mavl_logger{ara::log::LoggingMenager::GetInstance()->CreateLogger("MAVL", "", ara::log::LogLevel::kWarn)},
          someip_logger{ara::log::LoggingMenager::GetInstance()->CreateLogger("SOME", "", ara::log::LogLevel::kWarn)},
          primer_service_proxy{ara::core::InstanceSpecifier{kPrimer_service_path_name}},
          primer_service_handler{nullptr},
          servo_service_proxy{ara::core::InstanceSpecifier{kServo_service_path_name}},
          servo_service_handler{nullptr},
          env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path_name}},
          env_service_handler{nullptr},
          gps_service_proxy{ara::core::InstanceSpecifier{kGPS_service_path_name}},
          gps_service_handler{nullptr},
          main_service_handler{nullptr},
          main_service_proxy{ara::core::InstanceSpecifier{kMain_service_path_name}},
          engine_service_handler{nullptr},
          engine_service_proxy{ara::core::InstanceSpecifier{kEngine_service_path_name}},
          recovery_service_handler{nullptr},
          service_ipc_instance(kService_ipc_instance),
          service_udp_instance(kService_udp_instance) {
}

void RadioApp::SomeIpInit() {
    someip_logger.LogDebug() << "SomeIpInit started";
    this->env_service_proxy.StartFindService([this](auto handler) {
      someip_logger.LogDebug() << "Env service handler discovered";
      this->env_service_handler = handler;
      env_service_handler->newTempEvent_1.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to Env newTempEvent_1, status="
                             + std::to_string(static_cast<int>(status));
        env_service_handler->newTempEvent_1.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_1.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Env newTempEvent_1 sample: "
                               + std::to_string(res.Value());
          event_data->SetTemp(0, res.Value());
        });
      });
      env_service_handler->newTempEvent_2.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to Env newTempEvent_2, status="
                             + std::to_string(static_cast<int>(status));
        env_service_handler->newTempEvent_2.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_2.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Env newTempEvent_2 sample: "
                               + std::to_string(res.Value());
          event_data->SetTemp(1, res.Value());
        });
      });
      env_service_handler->newTempEvent_3.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to Env newTempEvent_3, status="
                             + std::to_string(static_cast<int>(status));
        env_service_handler->newTempEvent_3.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_3.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Env newTempEvent_3 sample: "
                               + std::to_string(res.Value());
          event_data->SetTemp(2, res.Value());
        });
      });
      env_service_handler->newPressEvent.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to Env newPressEvent, status="
                             + std::to_string(static_cast<int>(status));
        env_service_handler->newPressEvent.SetReceiveHandler([this] () {
          auto res = env_service_handler->newPressEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Env newPressEvent sample: "
                               + std::to_string(res.Value());
          event_data->SetPress(res.Value());
        });
      });
      env_service_handler->newBoardTempEvent1.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to Env newBoardTempEvent1, status="
                             + std::to_string(static_cast<int>(status));
        env_service_handler->newBoardTempEvent1.SetReceiveHandler([this] () {
          auto res = env_service_handler->newBoardTempEvent1.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Env newBoardTempEvent1 sample: "
                               + std::to_string(res.Value());
          event_data->SetComputerTemp(BoardType_e::EB, 0,  res.Value());
        });
      });
      env_service_handler->newBoardTempEvent2.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to Env newBoardTempEvent2, status="
                             + std::to_string(static_cast<int>(status));
        env_service_handler->newBoardTempEvent2.SetReceiveHandler([this] () {
          auto res = env_service_handler->newBoardTempEvent2.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Env newBoardTempEvent2 sample: "
                               + std::to_string(res.Value());
          event_data->SetComputerTemp(BoardType_e::EB, 1,  res.Value());
        });
      });
      env_service_handler->newBoardTempEvent3.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to Env newBoardTempEvent3, status="
                             + std::to_string(static_cast<int>(status));
        env_service_handler->newBoardTempEvent3.SetReceiveHandler([this] () {
          auto res = env_service_handler->newBoardTempEvent3.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Env newBoardTempEvent3 sample: "
                               + std::to_string(res.Value());
          event_data->SetComputerTemp(BoardType_e::EB, 2,  res.Value());
        });
      });
    });
    this->gps_service_proxy.StartFindService([this](auto handler) {
      someip_logger.LogDebug() << "GPS service handler discovered";
      this->gps_service_handler = handler;
      gps_service_handler->GPSStatusEvent.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to GPSStatusEvent, status="
                             + std::to_string(static_cast<int>(status));
        gps_service_handler->GPSStatusEvent.SetReceiveHandler([this] () {
          auto res_opt = gps_service_handler->GPSStatusEvent.GetNewSamples();
          if (!res_opt.HasValue()) {
            return;
          }
          auto res = res_opt.Value();
          someip_logger.LogDebug() << "GPSStatusEvent sample: lon="
                               + std::to_string(res.longitude)
                               + ", lat=" + std::to_string(res.latitude)
                               + ", alt=" + std::to_string(res.altitude);
          event_data->SetGPS(res.longitude, res.latitude, res.altitude);
        });
      });
    });
    this->servo_service_proxy.StartFindService([this](auto handler) {
      someip_logger.LogDebug() << "Servo service handler discovered";
      this->servo_service_handler = handler;
      servo_service_handler->ServoStatusEvent.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to ServoStatusEvent, status="
                             + std::to_string(static_cast<int>(status));
        servo_service_handler->ServoStatusEvent.SetReceiveHandler([this] () {
          auto res = servo_service_handler->ServoStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "ServoStatusEvent sample: "
                               + std::to_string(res.Value());
          event_data->SetActuatorState(SIMBA_ROCKET_MAIN_VALVE, res.Value());
        });
      });
      servo_service_handler->ServoVentStatusEvent.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to ServoVentStatusEvent, status="
                             + std::to_string(static_cast<int>(status));
        servo_service_handler->ServoVentStatusEvent.SetReceiveHandler([this] () {
          auto res = servo_service_handler->ServoVentStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "ServoVentStatusEvent sample: "
                               + std::to_string(res.Value());
          event_data->SetActuatorState(SIMBA_ROCKET_VENT_VALVE, res.Value());
        });
      });
      servo_service_handler->ServoDumpStatusEvent.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to ServoDumpStatusEvent, status="
                             + std::to_string(static_cast<int>(status));
        servo_service_handler->ServoDumpStatusEvent.SetReceiveHandler([this] () {
          auto res = servo_service_handler->ServoDumpStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "ServoDumpStatusEvent sample: "
                               + std::to_string(res.Value());
          event_data->SetActuatorState(SIMBA_ROCKET_DUMP_VALVE, res.Value());
        });
      });
    });
    // TODO(matikrajek42@gmail.com) add RECOVERY_SERVO, RECOVERY_LINECUTTER, ROCKET_CAMERAS
    this->main_service_proxy.StartFindService([this](auto handler) {
      someip_logger.LogDebug() << "Main service handler discovered";
      this->main_service_handler = handler;
      main_service_handler->CurrentModeStatusEvent.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to Main CurrentModeStatusEvent, status="
                             + std::to_string(static_cast<int>(status));
        main_service_handler->CurrentModeStatusEvent.SetReceiveHandler([this] () {
          auto res = main_service_handler->CurrentModeStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Main CurrentModeStatusEvent sample: "
                               << core::rocketState::to_string(static_cast<RocketState_t>(res.Value()));;
          this->event_data->SetComputerState(BoardType_e::MB,
                                  static_cast<RocketState_t>(res.Value()));
        });
      });
    });
    this->engine_service_proxy.StartFindService(([this](auto handler) {
      someip_logger.LogDebug() << "Engine service handler discovered";
      this->engine_service_handler = handler;
      engine_service_handler->CurrentMode.Subscribe(1, [this](const uint8_t status) {
        someip_logger.LogDebug() << "Subscribed to Engine CurrentMode, status=" << std::to_string(status);
        engine_service_handler->CurrentMode.SetReceiveHandler([this] () {
          auto res = engine_service_handler->CurrentMode.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Engine CurrentMode sample: "
                               << core::rocketState::to_string(static_cast<RocketState_t>(res.Value()));
          this->event_data->SetComputerState(BoardType_e::EB,
                                  static_cast<RocketState_t>(res.Value()));
        });
      });
    }));
    // TODO(matikrajek42@gmail.com) Write MB Temp After GrKo write Env App for FC
  }
}  // namespace apps
}  // namespace srp
