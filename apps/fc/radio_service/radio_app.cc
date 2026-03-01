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
#include <utility>
#include <vector>
#include "simba/mavlink.h"
#include "simba/simba.h"
#include "apps/fc/radio_service/radio_app.h"
#include "ara/log/log.h"
#include "core/common/condition.h"

namespace srp {
namespace apps {
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
  static constexpr auto KGPS_UART_path = "/dev/ttyS1";
  static constexpr auto KGPS_UART_baudrate = B115200;
  static constexpr auto kSystemId = 1;
  static constexpr auto kComponentId = 200;
  static constexpr auto kTime = 990;  // Should be 1 Hz but better make it 1.1Hz than 0.9 wchich can trigger error on GS
  static constexpr auto kSendWaifAfterMs = 50;  // Work ok but if you have more than
                                                // 15 Frames in Transmiting Loop you need
                                                // to reduce sending frequency or this delay

  static constexpr std::pair<uint8_t, core::rocketState::RocketState_t> gs_rocket_state_mapping[] = {
        {SIMBA_GS_ABORT,  core::rocketState::RocketState_t::ABORT},
        {SIMBA_GS_DISARM, core::rocketState::RocketState_t::DISARM},
        {SIMBA_GS_ARM,    core::rocketState::RocketState_t::ARM},
        {SIMBA_GS_LAUNCH, core::rocketState::RocketState_t::LAUNCH}
    };
  using RocketState_t = core::rocketState::RocketState_t;
}  // namespace

void RadioApp::TransmittingLoop(const std::stop_token& token) {
  mavlink_message_t msg;
  uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
  if (!timestamp_->Init()) {
    ara::log::LogWarn() << "Cant initialize app time";
  }
  event_data = EventData::GetInstance();
  auto send = [&](auto pack_func) {
    pack_func();
    uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);
    mavl_logger.LogDebug() << std::vector<uint8_t>(buffer, buffer + len);
    uart_->Write(std::vector<uint8_t>(buffer, buffer + len));
    core::condition::wait_for(std::chrono::milliseconds(kSendWaifAfterMs), token);
  };
  while (!token.stop_requested()) {
    auto start = std::chrono::high_resolution_clock::now();
    {
      // Send Heartbeat msg
      auto val = timestamp_->GetNewTimeStamp();
      if (val.has_value()) {
        send([&] {
          mavlink_msg_simba_rocket_heartbeat_pack(kSystemId, kComponentId, &msg,
                    static_cast<uint64_t>(val.value()), event_data->GetMBState(),
                    event_data->GetEBState(), event_data->GetActuatorStates());
        });
      } else {
        ara::log::LogWarn() << "Cant Get Timestamp";
      }

      send([&] {
        mavlink_msg_simba_computer_temperature_pack(kSystemId, kComponentId, &msg,
            event_data->GetEBTemp(), event_data->GetMBTemp());
      });

      // Send Max Altitude MSG
      send([&] {
        mavlink_msg_simba_max_altitude_pack(kSystemId, kComponentId, &msg, event_data->GetMaxAltitude());
      });


      // TODO(matikrajek42@gmail.com) Add send IMU data

      // Send Tank Temps
      send([&] { mavlink_msg_simba_tank_temperature_pack(kSystemId, kComponentId, &msg,
              static_cast<int16_t>(event_data->GetTemp1()), static_cast<int16_t>(event_data->GetTemp2()),
              static_cast<int16_t>(event_data->GetTemp3())); });

      // Send Tank pressure
      send([&] { mavlink_msg_simba_tank_pressure_pack(kSystemId, kComponentId, &msg,
                                                  static_cast<uint16_t>(event_data->GetPress())); });

      //  Send
      send([&] { mavlink_msg_simba_gps_pack(kSystemId, kComponentId, &msg,
                            event_data->GetGPSLat(), event_data->GetGPSLon(), event_data->GetGPSAlt()); });
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    core::condition::wait_for(std::chrono::milliseconds(kTime - duration.count()), token);  // 1 Hz
  }
}

std::optional<RocketState_t> RadioApp::GetReqRocketStateFromGSFlags(const uint8_t flags) {
    for (const auto& [mask, state] : gs_rocket_state_mapping) {
        if (flags & mask) return state;
    }

    return std::nullopt;
}

void RadioApp::ListeningLoop(const std::stop_token& token) {
  mavlink_message_t msg;
  mavlink_status_t status;
  while (!token.stop_requested()) {
    auto bytes_opt = uart_->Read(1);
    if (!bytes_opt.has_value()) {
      continue;
    }
    if (!mavlink_parse_char(MAVLINK_COMM_0, bytes_opt.value()[0], &msg, &status)) {
      continue;
    }
    switch (msg.msgid) {
      case MAVLINK_MSG_ID_SIMBA_ACTUATOR_CMD:
        // TODO(matikrajek42@gmail.com) Add support for CMD command
        break;
      case MAVLINK_MSG_ID_SIMBA_GS_HEARTBEAT:
        auto timestamp = mavlink_msg_simba_gs_heartbeat_get_timestamp(&msg);
        auto values = mavlink_msg_simba_gs_heartbeat_get_values(&msg);

        auto req_state = GetReqRocketStateFromGSFlags(values);
        if (req_state.has_value()) {
          if (req_state.value() != event_data->GetEBState() &&
                  req_state.value() != event_data->GetMBState()) {
            main_service_handler->setMode(static_cast<uint8_t>(req_state.value()));
            engine_service_handler->SetMode(static_cast<uint8_t>(req_state.value()));
          }
        }
        if (values & SIMBA_GS_VENT_VALVE != event_data->GetActuatorStates() & SIMBA_GS_VENT_VALVE) {
          servo_service_handler->SetVentServoValue(values & SIMBA_GS_VENT_VALVE);
        }
        if (values & SIMBA_GS_DUMP_VALVE != event_data->GetActuatorStates() & SIMBA_GS_DUMP_VALVE) {
          servo_service_handler->SetDumpValue(values & SIMBA_GS_DUMP_VALVE);
        }
        // TODO(matikrajek42@gmail.com) Add missing Cameras handler
    }
  }
}

int RadioApp::Run(const std::stop_token& token) {
  std::jthread transmitting_thread([this](const std::stop_token& t) {
    this->TransmittingLoop(t);
    });
  std::jthread listening_thread([this](const std::stop_token& t) {
    this->ListeningLoop(t);
  });
  core::condition::wait(token);
  service_ipc->StopOffer();
  service_udp->StopOffer();
  uart_->Close();
  return core::ErrorCode::kOk;
}

void RadioApp::InitUart(std::unique_ptr<core::uart::IUartDriver> uart) {
    this->uart_ = std::move(uart);
}

void RadioApp::InitTimestamp(std::unique_ptr<core::timestamp::ITimestampController> timestamp) {
    this->timestamp_ = std::move(timestamp);
}

int RadioApp::Initialize(const std::map<ara::core::StringView,
   ara::core::StringView> parms) {
    if (!this->uart_) {
        auto uart_d = std::make_unique<core::uart::UartDriver>();
        InitUart(std::move(uart_d));
    }
    if (!this->uart_->Open(KGPS_UART_path, KGPS_UART_baudrate)) {
        return 1;
      }
    if (!this->timestamp_) {
        InitTimestamp(std::make_unique<core::timestamp::TimestampController>());
    }
    service_ipc = std::make_unique<apps::RadioServiceSkeleton>(service_ipc_instance);
    service_udp = std::make_unique<apps::RadioServiceSkeleton>(service_udp_instance);
    service_ipc->StartOffer();
    service_udp->StartOffer();
    this->SomeIpInit();
    return core::ErrorCode::kOk;
}
void RadioApp::SomeIpInit() {
    this->env_service_proxy.StartFindService([this](auto handler) {
      this->env_service_handler = handler;
      env_service_handler->newTempEvent_1.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newTempEvent_1.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_1.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetTemp1(res.Value());
        });
      });
      env_service_handler->newTempEvent_2.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newTempEvent_2.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_2.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetTemp2(res.Value());
        });
      });
      env_service_handler->newTempEvent_3.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newTempEvent_3.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_3.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetTemp3(res.Value());
        });
      });
      env_service_handler->newPressEvent.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newPressEvent.SetReceiveHandler([this] () {
          auto res = env_service_handler->newPressEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetPress(res.Value());
        });
      });
      env_service_handler->newBoardTempEvent1.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newBoardTempEvent1.SetReceiveHandler([this] () {
          auto res = env_service_handler->newBoardTempEvent1.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetEBTemp(1, res.Value());
        });
      });
      env_service_handler->newBoardTempEvent2.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newBoardTempEvent2.SetReceiveHandler([this] () {
          auto res = env_service_handler->newBoardTempEvent2.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetEBTemp(2, res.Value());
        });
      });
      env_service_handler->newBoardTempEvent3.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newBoardTempEvent3.SetReceiveHandler([this] () {
          auto res = env_service_handler->newBoardTempEvent3.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetEBTemp(3, res.Value());
        });
      });
    });
    this->gps_service_proxy.StartFindService([this](auto handler) {
      this->gps_service_handler = handler;
      gps_service_handler->GPSStatusEvent.Subscribe(1, [this](const uint8_t status) {
        gps_service_handler->GPSStatusEvent.SetReceiveHandler([this] () {
          auto res_opt = gps_service_handler->GPSStatusEvent.GetNewSamples();
          if (!res_opt.HasValue()) {
            return;
          }
          auto res = res_opt.Value();
          event_data->SetGPS(res.longitude, res.latitude, res.altitude);
        });
      });
    });
    this->servo_service_proxy.StartFindService([this](auto handler) {
      this->servo_service_handler = handler;
      servo_service_handler->ServoStatusEvent.Subscribe(1, [this](const uint8_t status) {
        servo_service_handler->ServoStatusEvent.SetReceiveHandler([this] () {
          auto res = servo_service_handler->ServoStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetActuatorState(SIMBA_ROCKET_MAIN_VALVE, res.Value());
        });
      });
      servo_service_handler->ServoVentStatusEvent.Subscribe(1, [this](const uint8_t status) {
        servo_service_handler->ServoVentStatusEvent.SetReceiveHandler([this] () {
          auto res = servo_service_handler->ServoVentStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetActuatorState(SIMBA_ROCKET_VENT_VALVE, res.Value());
        });
      });
      servo_service_handler->ServoDumpStatusEvent.Subscribe(1, [this](const uint8_t status) {
        servo_service_handler->ServoDumpStatusEvent.SetReceiveHandler([this] () {
          auto res = servo_service_handler->ServoDumpStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetActuatorState(SIMBA_ROCKET_DUMP_VALVE, res.Value());
        });
      });
    });
    // TODO(matikrajek42@gmail.com) add RECOVERY_SERVO, RECOVERY_LINECUTTER, ROCKET_CAMERAS
    this->main_service_proxy.StartFindService([this](auto handler) {
      this->main_service_handler = handler;
      main_service_handler->CurrentModeStatusEvent.Subscribe(1, [this](const uint8_t status) {
        main_service_handler->CurrentModeStatusEvent.SetReceiveHandler([this] () {
          auto res = main_service_handler->CurrentModeStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          this->event_data->SetMBState(static_cast<core::rocketState::RocketState_t>(res.Value()));
        });
      });
    });
    this->engine_service_proxy.StartFindService(([this](auto handler) {
      this->engine_service_handler = handler;
      engine_service_handler->CurrentMode.Subscribe(1, [this](const uint8_t status) {
        engine_service_handler->CurrentMode.SetReceiveHandler([this] () {
          auto res = engine_service_handler->CurrentMode.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          this->event_data->SetEBState(static_cast<core::rocketState::RocketState_t>(res.Value()));
        });
      });
    }));
    // TODO(matikrajek42@gmail.com) Write MB Temp After GrKo write Env App for FC
  }
RadioApp::~RadioApp() {
}
RadioApp::RadioApp(): service_ipc_instance(kService_ipc_instance),
                        service_udp_instance(kService_udp_instance),
env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path_name}},
env_service_handler{nullptr},
gps_service_proxy{ara::core::InstanceSpecifier{kGPS_service_path_name}},
gps_service_handler{nullptr},
primer_service_proxy{ara::core::InstanceSpecifier{kPrimer_service_path_name}},
primer_service_handler{nullptr},
servo_service_proxy{ara::core::InstanceSpecifier{kServo_service_path_name}},
servo_service_handler{nullptr},
main_service_proxy{ara::core::InstanceSpecifier{kMain_service_path_name}},
main_service_handler{nullptr},
recovery_service_handler{nullptr},
engine_service_handler{nullptr},
engine_service_proxy{ara::core::InstanceSpecifier{kEngine_service_path_name}},
mavl_logger{ara::log::LoggingMenager::GetInstance()->CreateLogger("MAVL", "", ara::log::LogLevel::kInfo)}
{
}
}  // namespace apps
}  // namespace srp
