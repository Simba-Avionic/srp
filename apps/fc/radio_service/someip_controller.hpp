/**
 * @file someip_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_FC_RADIO_SERVICE_SOMEIP_CONTROLLER_HPP_
#define APPS_FC_RADIO_SERVICE_SOMEIP_CONTROLLER_HPP_
#include "apps/fc/radio_service/event_data.h"
#include "apps/fc/radio_service/someip_controller.hpp"
#include "apps/fc/recovery_service/parachute_controller.hpp"
#include "apps/ec/primer_service/controller/primer_controller.hpp"

#include "srp/env/EnvApp/EnvAppHandler.h"
#include "srp/env/EnvAppFc/EnvAppFcHandler.h"
#include "srp/apps/SysStatService/SysStatServiceHandler.h"
#include "srp/apps/FcSysStatService/FcSysStatServiceHandler.h"
#include "srp/apps/GPSService/GPSServiceHandler.h"
#include "srp/apps/PrimerService/PrimerServiceHandler.h"
#include "srp/apps/ServoService/ServoServiceHandler.h"
#include "srp/apps/MainService/MainServiceHandler.h"
#include "srp/apps/EngineService/EngineServiceHandler.h"
#include "srp/apps/RecoveryService/RecoveryServiceHandler.h"

namespace srp {
namespace apps {
namespace radio {

using RocketState_t = core::rocketState::RocketState_t;
using PrimerState_t = srp::primer::PrimerState_t;
using timepoint = std::chrono::_V2::system_clock::time_point;

class SomeIPController {
 private:
  const ara::log::Logger& someip_logger;
  std::shared_ptr<EventData> event_data;

  PrimerServiceProxy primer_service_proxy;
  std::shared_ptr<PrimerServiceHandler> primer_service_handler;
  ServoServiceProxy servo_service_proxy;
  std::shared_ptr<ServoServiceHandler> servo_service_handler;
  env::EnvAppProxy env_service_proxy;
  std::shared_ptr<env::EnvAppHandler> env_service_handler;
  srp::env::EnvAppFcProxy env_fc_service_proxy;
  std::shared_ptr<env::EnvAppFcHandler> env_fc_service_handler;
  GPSServiceProxy gps_service_proxy;
  std::shared_ptr<GPSServiceHandler> gps_service_handler;
  std::shared_ptr<MainServiceHandler> main_service_handler;
  MainServiceProxy main_service_proxy;
  std::shared_ptr<EngineServiceHandler> engine_service_handler;
  EngineServiceProxy engine_service_proxy;
  std::shared_ptr<RecoveryServiceHandler> recovery_service_handler;
  RecoveryServiceProxy recovery_service_proxy;
  void SomeIpInit();

 public:
  SomeIPController();
  ~SomeIPController();
  std::shared_ptr<MainServiceHandler> GetMainServiceHandler();
  std::shared_ptr<EngineServiceHandler> GetEngineServiceHandler();
  std::shared_ptr<ServoServiceHandler> GetServoServiceHandler();

};

}  // namespace radio
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_SOMEIP_CONTROLLER_HPP_
