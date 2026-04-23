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
#include "apps/fc/radio_service/event_data.h"
#include "srp/apps/RadioServiceSkeleton.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "apps/fc/radio_service/telemetry_provider.hpp"
#include "apps/fc/radio_service/someip_controller.hpp"
#include "apps/fc/radio_service/radio_controller.hpp"
#include "apps/fc/radio_service/gs_hb_guider.hpp"


namespace srp {
namespace apps {
using RocketState_t = core::rocketState::RocketState_t;
using timepoint = std::chrono::_V2::system_clock::time_point;

class RadioApp : public ara::exec::AdaptiveApplication {
 private:
  radio::TelemetryProvider telemetry_provider;
  radio::SomeIPController someip_controller;
  radio::MavRadioController radio_controller;
  gpio::GPIOController gpio_;
  radio::GSHeartbeatGuard heartbeat_controller;

  timepoint last_received_hb;
  bool IsStateChangeApproved(const RocketState_t req_state);
  void WaitUntillTimeEnd(const timepoint& start, const uint32_t req_loop_time,
                                                  const std::stop_token& token);

  std::pair<RocketState_t, uint8_t> req_rocket_state;
  std::shared_ptr<EventData> event_data;

  apps::RadioServiceSkeleton service_ipc;
  apps::RadioServiceSkeleton service_udp;


  void OnActuatorCMD(const mavlink_message_t& msg);
  void HBHangleActuators(const uint8_t values);
  void HBHangleState(const uint8_t values);
  void OnGSHEARTBEAT(const mavlink_message_t& msg);
  void TransmittingLoop(const std::stop_token& token);
  void RxMsgCallback(const mavlink_message_t& msg);

 public:
  int Run(const std::stop_token& token) override;
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                                                                parms) override;
  RadioApp();
};
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_RADIO_APP_H_
