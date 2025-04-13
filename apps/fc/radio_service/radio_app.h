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
#include "lib/simba/mavlink.h"
#include "srp/env/EnvApp/EnvAppHandler.h"
#include "srp/apps/GPSService/GPSServiceHandler.h"
#include "srp/apps/PrimerService/PrimerServiceHandler.h"
#include "srp/apps/ServoService/ServoServiceHandler.h"
#include "core/timestamp/timestamp_driver.hpp"
#include "srp/apps/RadioServiceSkeleton.h"
#include "core/uart/uart_driver.hpp"


namespace srp {
namespace apps {
class RadioApp final : public ara::exec::AdaptiveApplication {
 private:
  __mavlink_simba_tank_temperature_t temp;
  __mavlink_simba_tank_pressure_t press;
  __mavlink_simba_gps_t gps;
  __mavlink_simba_actuator_t actuator;
  std::mutex mutex_;
  PrimerServiceProxy primer_service_proxy;
  std::shared_ptr<PrimerServiceHandler> primer_service_handler;
  ServoServiceProxy servo_service_proxy;
  std::shared_ptr<ServoServiceHandler> servo_service_handler;
  env::EnvAppProxy env_service_proxy;
  std::shared_ptr<env::EnvAppHandler> env_service_handler;
  GPSServiceProxy gps_service_proxy;
  std::shared_ptr<GPSServiceHandler> gps_service_handler;
  const ara::core::InstanceSpecifier service_ipc_instance;
  const ara::core::InstanceSpecifier service_udp_instance;
  std::unique_ptr<apps::RadioServiceSkeleton> service_ipc;
  std::unique_ptr<apps::RadioServiceSkeleton> service_udp;
  std::unique_ptr<core::uart::IUartDriver> uart_;
  void SomeIpInit();
  void TransmittingLoop(const std::stop_token& token);

 public:
  int Run(const std::stop_token& token) override;
  void Init(std::unique_ptr<core::uart::IUartDriver> uart);
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
    parms) override;
  ~RadioApp();
  RadioApp();
};
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_RADIO_APP_H_
