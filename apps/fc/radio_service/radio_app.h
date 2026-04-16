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
#include <atomic>
#include <vector>
#include <map>
#include <utility>

#include "ara/exec/adaptive_application.h"
#include "simba/mavlink.h"
#include "apps/fc/radio_service/event_data.h"
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
#include "core/timestamp/timestamp_driver.hpp"
#include "srp/apps/RadioServiceSkeleton.h"
#include "apps/fc/radio_service/mavlink_uart_driver.hpp"
#include "ara/log/logging_menager.h"
#include "core/common/wait_queue.h"
#include "apps/ec/primer_service/controller/primer_controller.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"

namespace srp {
namespace apps {
using RocketState_t = core::rocketState::RocketState_t;
using PrimerState_t = srp::primer::PrimerState_t;

class RadioApp : public ara::exec::AdaptiveApplication {
 private:
  const ara::log::Logger& mavl_logger;
  const ara::log::Logger& someip_logger;
  gpio::GPIOController gpio_;

  bool IsStateChangeApproved(const RocketState_t req_state);

  core::WaitQueue<std::vector<uint8_t>, 20> UartTxQueue;

  std::pair<RocketState_t, uint8_t> req_rocket_state;

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
  const ara::core::InstanceSpecifier service_ipc_instance;
  const ara::core::InstanceSpecifier service_udp_instance;
  std::unique_ptr<apps::RadioServiceSkeleton> service_ipc;
  std::unique_ptr<apps::RadioServiceSkeleton> service_udp;
  mutable std::mutex handler_mtx_;
  std::atomic_bool shutting_down_{false};

  core::uart::MavUartDriver uart_;

  core::timestamp::TimestampController timestamp_;
  void SomeIpInit();
  void OnActuatorCMD(const mavlink_message_t& msg);
  void HBHangleActuators(const uint8_t values);
  void HBHangleState(const uint8_t values);
  void OnGSHEARTBEAT(const mavlink_message_t& msg);

  std::optional<RocketState_t> GetReqRocketStateFromGSFlags(const uint8_t flags);
  uint8_t RocketStateToMavlinkState(const RocketState_t state) const noexcept;
  uint8_t PrimerStateToMavlinState(const PrimerState_t state) const noexcept;

 protected:
  void TransmittingLoop(const std::stop_token& token);
  void ListeningLoop(const std::stop_token& token);
  void ServicePollingLoop(const std::stop_token& token);
  std::shared_ptr<EventData> event_data;

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
