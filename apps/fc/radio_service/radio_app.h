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
#include "core/timestamp/timestamp_driver.hpp"
#include "srp/apps/RadioServiceSkeleton.h"
#include "apps/fc/radio_service/mavlink_uart_driver.hpp"
#include "ara/log/logging_menager.h"
#include "core/common/wait_queue.h"
#include "apps/ec/primer_service/controller/primer_controller.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "apps/fc/radio_service/telemetry_provider.hpp"
#include "apps/fc/radio_service/someip_controller.hpp"


namespace srp {
namespace apps {
using RocketState_t = core::rocketState::RocketState_t;
using PrimerState_t = srp::primer::PrimerState_t;
using timepoint = std::chrono::_V2::system_clock::time_point;

class RadioApp : public ara::exec::AdaptiveApplication {
 private:
  radio::TelemetryProvider telemetry_provider;
  radio::SomeIPController someip_controller;

  const ara::log::Logger& mavl_logger;
  gpio::GPIOController gpio_;

  timepoint last_received_hb;

  bool IsStateChangeApproved(const RocketState_t req_state);
  void WaitUntillTimeEnd(const timepoint& start, const uint32_t req_loop_time,
                                                  const std::stop_token& token);

  core::WaitQueue<std::vector<uint8_t>, 20> UartTxQueue;

  std::pair<RocketState_t, uint8_t> req_rocket_state;

  const ara::core::InstanceSpecifier service_ipc_instance;
  const ara::core::InstanceSpecifier service_udp_instance;
  std::unique_ptr<apps::RadioServiceSkeleton> service_ipc;
  std::unique_ptr<apps::RadioServiceSkeleton> service_udp;

  core::uart::MavUartDriver uart_;

  void OnActuatorCMD(const mavlink_message_t& msg);
  void HBHangleActuators(const uint8_t values);
  void HBHangleState(const uint8_t values);
  void OnGSHEARTBEAT(const mavlink_message_t& msg);

  std::optional<RocketState_t> GetReqRocketStateFromGSFlags(const uint8_t flags);

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
