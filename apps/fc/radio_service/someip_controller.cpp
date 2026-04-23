/**
 * @file someip_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "apps/fc/radio_service/someip_controller.hpp"
#include <memory>
#include <vector>
#include <chrono>  // NOLINT
#include "ara/log/logging_menager.h"

namespace srp {
namespace apps {
namespace radio {

namespace {
  static constexpr auto kEnv_service_path_name =      "srp/apps/RadioApp/EnvApp";
  static constexpr auto kGPS_service_path_name =      "srp/apps/RadioApp/GPSService";
  static constexpr auto kPrimer_service_path_name =   "srp/apps/RadioApp/PrimerService";
  static constexpr auto kServo_service_path_name =    "srp/apps/RadioApp/ServoService";
  static constexpr auto kRecovery_service_path_name = "srp/apps/RadioApp/RecoveryService";
  static constexpr auto kMain_service_path_name =     "srp/apps/RadioApp/MainService";
  static constexpr auto kEngine_service_path_name =   "srp/apps/RadioApp/EngineService";
  static constexpr auto kEnv_fc_service_path_name =   "srp/apps/RadioApp/EnvAppFc";
}  // namespace

SomeIPController::SomeIPController():
    someip_logger{ara::log::LoggingMenager::GetInstance()->CreateLogger("SOME", "", ara::log::LogLevel::kWarn)},
    event_data(EventData::GetInstance()),
    primer_service_proxy{ara::core::InstanceSpecifier{kPrimer_service_path_name}},
    primer_service_handler{nullptr},
    servo_service_proxy{ara::core::InstanceSpecifier{kServo_service_path_name}},
    servo_service_handler{nullptr},
    env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path_name}},
    env_service_handler{nullptr},
    env_fc_service_proxy{ara::core::InstanceSpecifier{kEnv_fc_service_path_name}},
    env_fc_service_handler{nullptr},
    gps_service_proxy{ara::core::InstanceSpecifier{kGPS_service_path_name}},
    gps_service_handler{nullptr},
    main_service_handler{nullptr},
    main_service_proxy{ara::core::InstanceSpecifier{kMain_service_path_name}},
    engine_service_handler{nullptr},
    engine_service_proxy{ara::core::InstanceSpecifier{kEngine_service_path_name}},
    recovery_service_handler{nullptr},
    recovery_service_proxy{ara::core::InstanceSpecifier{kRecovery_service_path_name}} {
    SomeIpInit();
}
std::shared_ptr<MainServiceHandler> SomeIPController::GetMainServiceHandler() {
    return main_service_handler;
}
std::shared_ptr<EngineServiceHandler> SomeIPController::GetEngineServiceHandler() {
    return engine_service_handler;
}

std::shared_ptr<ServoServiceHandler> SomeIPController::GetServoServiceHandler() {
    return servo_service_handler;
}

void SomeIPController::SomeIpInit() {
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
          event_data->SetActuatorState(SIMBA_ACTUATOR_FLAGS_MAIN_VALVE, res.Value());
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
          event_data->SetActuatorState(SIMBA_ACTUATOR_FLAGS_VENT_VALVE, res.Value());
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
          event_data->SetActuatorState(SIMBA_ACTUATOR_FLAGS_DUMP_VALVE, res.Value());
        });
      });
    });
    this->recovery_service_proxy.StartFindService([this] (auto handler){
      someip_logger.LogDebug() << "Recovery service handler discovered";
      this->recovery_service_handler = handler;
      recovery_service_handler->NewParachuteStatusEvent.Subscribe(1, [this](const uint8_t status){
        someip_logger.LogDebug() << "Subscribed to ParachuteStatusEvent, status="
                             + std::to_string(static_cast<int>(status));
        recovery_service_handler->NewParachuteStatusEvent.SetReceiveHandler([this] (){
          auto res = recovery_service_handler->NewParachuteStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "NewParachuteStatusEvent sample: "
                      << std::to_string(res.Value());
          switch (static_cast<srp::apps::recovery::ParachuteState_t> (res.Value())) {
            case recovery::ParachuteState_t::CLOSED:
              this->event_data->SetActuatorState(SIMBA_ACTUATOR_FLAGS_RECOVERY_SERVO, 0);
              this->event_data->SetActuatorState(SIMBA_ACTUATOR_FLAGS_RECOVERY_LINECUTTER, 0);
              break;
            case recovery::ParachuteState_t::OPEN_REEFED:
            case recovery::ParachuteState_t::OPENING_REEFED:
              this->event_data->SetActuatorState(SIMBA_ACTUATOR_FLAGS_RECOVERY_SERVO, 1);
              this->event_data->SetActuatorState(SIMBA_ACTUATOR_FLAGS_RECOVERY_LINECUTTER, 0);
              break;
            case recovery::ParachuteState_t::OPEN_UNREEFED:
            case recovery::ParachuteState_t::OPENING_UNREEFED:
              this->event_data->SetActuatorState(SIMBA_ACTUATOR_FLAGS_RECOVERY_SERVO, 1);
              this->event_data->SetActuatorState(SIMBA_ACTUATOR_FLAGS_RECOVERY_LINECUTTER, 1);
              break;
            default:
              break;
          }
        });
      });
    });
    this->env_fc_service_proxy.StartFindService([this](auto handler) {
      someip_logger.LogDebug() << "Env Flight Computer service handler discovered";
      this->env_fc_service_handler = handler;
      env_fc_service_handler->newBME280Event.Subscribe(1, [this](const uint8_t status){
        someip_logger.LogDebug() << "Subscribed to BME280 event, status="
                             + std::to_string(static_cast<int>(status));
        env_fc_service_handler->newBME280Event.SetReceiveHandler([this] (){
          auto res = env_fc_service_handler->newBME280Event.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "Altitude sample: "
                      << std::to_string(res.Value().altitude);
          this->event_data->SetMaxAltitude(static_cast<int32_t>(res.Value().altitude));
        });
      });
      env_fc_service_handler->newBoardTempEvent_1.Subscribe(1, [this](const uint8_t status){
        someip_logger.LogDebug() << "Subscribed to newBoardTempEvent_1 event, status="
                             + std::to_string(static_cast<int>(status));
        env_fc_service_handler->newBoardTempEvent_1.SetReceiveHandler([this] (){
          auto res = env_fc_service_handler->newBoardTempEvent_1.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "newBoardTempEvent_1 sample: "
                      << std::to_string(res.Value());
          this->event_data->SetComputerTemp(BoardType_e::MB, 0, res.Value());
        });
      });
      env_fc_service_handler->newBoardTempEvent_2.Subscribe(1, [this](const uint8_t status){
        someip_logger.LogDebug() << "Subscribed to newBoardTempEvent_2 event, status="
                             + std::to_string(static_cast<int>(status));
        env_fc_service_handler->newBoardTempEvent_2.SetReceiveHandler([this] (){
          auto res = env_fc_service_handler->newBoardTempEvent_2.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "newBoardTempEvent_2 sample: "
                      << std::to_string(res.Value());
          this->event_data->SetComputerTemp(BoardType_e::MB, 1, res.Value());
        });
      });
      env_fc_service_handler->newBoardTempEvent_3.Subscribe(1, [this](const uint8_t status){
        someip_logger.LogDebug() << "Subscribed to newBoardTempEvent_3 event, status="
                             + std::to_string(static_cast<int>(status));
        env_fc_service_handler->newBoardTempEvent_3.SetReceiveHandler([this] (){
          auto res = env_fc_service_handler->newBoardTempEvent_3.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          someip_logger.LogDebug() << "newBoardTempEvent_3 sample: "
                      << std::to_string(res.Value());
          this->event_data->SetComputerTemp(BoardType_e::MB, 2, res.Value());
        });
      });
    });

    // TODO(matikrajek42@gmail.com)  ROCKET_CAMERAS
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
                               << core::rocketState::to_string(static_cast<RocketState_t>(res.Value()));
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
    this->primer_service_proxy.StartFindService([this](auto handler) {
      someip_logger.LogDebug() << "Primer service handler discovered";
      this->primer_service_handler = handler;
      primer_service_handler->primeStatusEvent.SetReceiveHandler([this] () {
        const auto res = primer_service_handler->primeStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Primer state sample: "
                               << std::to_string(res.Value());
        this->event_data->SetPrimerState(res.Value());
      });
    });
    // TODO(matikrajek42@gmail.com) Write MB Temp After GrKo write Env App for FC
}

SomeIPController::~SomeIPController() {
  env_service_proxy.StopFindService();
  gps_service_proxy.StopFindService();
  primer_service_proxy.StopFindService();
  servo_service_proxy.StopFindService();
  recovery_service_proxy.StopFindService();
  main_service_proxy.StopFindService();
  engine_service_proxy.StopFindService();
  env_fc_service_proxy.StopFindService();
}

}  // namespace radio
}  // namespace apps
}  // namespace srp
