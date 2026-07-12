/**
 * @file servoService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Implementation of secondary EC Servo Service
 * @version 0.1
 * @date 2026-07-01
 *
 * @copyright Copyright (c) 2026
 */
#include "apps/sec_ec/ServoService/servoService.hpp"

#include <memory>
#include <optional>
#include <string>

#include "ara/log/log.h"
#include "core/common/condition.h"

namespace srp {
namespace sec_ec {

namespace {
  static constexpr auto kEventIntervalMs = std::chrono::milliseconds(1000);

  static constexpr auto kEthanolMainValveID = 63;
  static constexpr auto kEthanolVentValveID = 64;

  static constexpr auto kIpcInstanceSpecifier =
      "srp/apps/secServoService/SecServoService_ipc";
  static constexpr auto kUdpInstanceSpecifier =
      "srp/apps/secServoService/SecServoService_udp";
  static constexpr auto kHeartBeatPinID = 4;
}  // namespace

SecServoService::SecServoService() {}

int SecServoService::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "SecServoService.Run: offering services";
  if (!servo_controller || !service_ipc || !service_udp) {
    ara::log::LogError() << "SecServoService.Run: service components are not initialized";
    return 1;
  }
  gpio_.SetPinValue(9, 1);
  gpio_.SetPinValue(5, 1);
  gpio_.SetPinValue(6, 1);
  gpio_.SetPinValue(7, 1);
  gpio_.SetPinValue(14, 1);
  gpio_.SetPinValue(8, 1);

  service_ipc->StartOffer();
  service_udp->StartOffer();

  auto update_servo_status =
      [&](std::uint8_t id, auto& ipc_event, auto& udp_event, const char* name,
          std::optional<uint8_t>& last_value) {
      auto val = this->servo_controller->ReadServoPosition(id);
      if (val.has_value()) {
          ipc_event.Update(val.value());
          udp_event.Update(val.value());
          if (!last_value.has_value() || last_value.value() != val.value()) {
            ara::log::LogInfo() << "SecServoService.Run: " << name
                                << " servo status changed to " << val.value();
            last_value = val.value();
          }
      } else {
          ara::log::LogWarn() << "SecServoService.Run: failed to read " << name
                              << " servo status";
      }
  };

  std::optional<uint8_t> last_main_state;
  std::optional<uint8_t> last_vent_state;

  while (!token.stop_requested()) {
    if (gpio_.SetPinValue(kHeartBeatPinID, 1, 500) != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "SecServoService::Run: failed to toggle heartbeat pin";
    }
    update_servo_status(kEthanolMainValveID,
                        service_ipc->newEthanolMainValveEvent,
                        service_udp->newEthanolMainValveEvent,
                        "eth_main", last_main_state);
    update_servo_status(kEthanolVentValveID,
                        service_ipc->newEthanolVentValveEvent,
                        service_udp->newEthanolVentValveEvent,
                        "eth_vent", last_vent_state);

    core::condition::wait_for(kEventIntervalMs, token);
  }

  ara::log::LogInfo() << "SecServoService.Run: stopping offers";

  service_ipc->StopOffer();
  service_udp->StopOffer();

  ara::log::LogInfo() << "SecServoService.Run: stopped";
  return 0;
}

int SecServoService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  ara::log::LogInfo() << "SecServoService.Initialize: start";

  if (parms.find("app_path") == parms.end()) {
      ara::log::LogError() << "SecServoService.Initialize: missing 'app_path' parameter";
      return 1;
  }

  this->servo_controller = std::make_shared<service::ServoController>();
  std::string app_path(parms.at("app_path"));

  ara::log::LogDebug() << "SecServoService.Initialize: using app path " << app_path;

  this->servo_controller->Init(app_path);

  service_ipc = std::make_unique<apps::MySecServoService>(
                ara::core::InstanceSpecifier(kIpcInstanceSpecifier), this->servo_controller);

  service_udp = std::make_unique<apps::MySecServoService>(
                ara::core::InstanceSpecifier(kUdpInstanceSpecifier), this->servo_controller);

  ara::log::LogInfo() << "SecServoService.Initialize: completed";
  return 0;
}

}  // namespace sec_ec
}  // namespace srp
