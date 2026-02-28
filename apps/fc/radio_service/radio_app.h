/**
 * @file radio_app.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef APPS_FC_RADIO_SERVICE_RADIO_APP_H_
#define APPS_FC_RADIO_SERVICE_RADIO_APP_H_

#include <memory>
#include <mutex>  // NOLINT
#include <map>

#include "ara/exec/adaptive_application.h"
#include "simba/mavlink.h"
#include "apps/fc/radio_service/event_data.h"
#include "srp/env/EnvApp/EnvAppHandler.h"
#include "srp/apps/GPSService/GPSServiceHandler.h"
#include "srp/apps/PrimerService/PrimerServiceHandler.h"
#include "srp/apps/ServoService/ServoServiceHandler.h"
#include "srp/apps/MainService/MainServiceHandler.h"
#include "srp/apps/RecoveryService/RecoveryServiceHandler.h"
#include "core/timestamp/timestamp_driver.hpp"
#include "srp/apps/RadioServiceSkeleton.h"
#include "core/uart/uart_driver.hpp"
#include "ara/log/logging_menager.h"

namespace srp {
namespace apps {
class RadioApp : public ara::exec::AdaptiveApplication {
 private:
  SIMBA_ROCKET_STATE current_state;
  const ara::log::Logger& mavl_logger;

  PrimerServiceProxy primer_service_proxy;
  std::shared_ptr<PrimerServiceHandler> primer_service_handler;
  ServoServiceProxy servo_service_proxy;
  std::shared_ptr<ServoServiceHandler> servo_service_handler;
  env::EnvAppProxy env_service_proxy;
  std::shared_ptr<env::EnvAppHandler> env_service_handler;
  GPSServiceProxy gps_service_proxy;
  std::shared_ptr<GPSServiceHandler> gps_service_handler;
  std::shared_ptr<MainServiceHandler> main_service_handler;
  MainServiceProxy main_service_proxy;
  std::shared_ptr<RecoveryServiceHandler> recovery_service_handler;
  const ara::core::InstanceSpecifier service_ipc_instance;
  const ara::core::InstanceSpecifier service_udp_instance;
  std::unique_ptr<apps::RadioServiceSkeleton> service_ipc;
  std::unique_ptr<apps::RadioServiceSkeleton> service_udp;

  std::unique_ptr<core::uart::IUartDriver> uart_;
  std::mutex uart_mutex_;

  std::unique_ptr<core::timestamp::ITimestampController> timestamp_;
  void SomeIpInit();

 protected:
  void InitUart(std::unique_ptr<core::uart::IUartDriver> uart);
  void InitTimestamp(std::unique_ptr<core::timestamp::ITimestampController> timestamp);

  void TransmittingLoop(const std::stop_token& token);
  void ListeningLoop(const std::stop_token& token);
  SIMBA_STATUS ActuatorCMD(uint8_t actuator_id, uint8_t value);
  std::shared_ptr<EventData> event_data;

  SIMBA_ROCKET_STATE GetStateFromMsg(const uint8_t values);

 public:
  int Run(const std::stop_token& token) override;
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
    parms) override;
  ~RadioApp();
  RadioApp();
};
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_RADIO_APP_H_
