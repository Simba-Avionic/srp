/**
 * @file servoService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Implementation of Servo Service
 * @version 0.2
 * @date 2024-03-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "apps/ec/ServoService/servoService.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "ara/log/log.h"
#include "core/common/condition.h"
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/i2c_service/controller/i2c_controller.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"

namespace srp {
namespace service {

namespace {
  static constexpr auto kEventIntervalMs = std::chrono::milliseconds(1000);

  static constexpr auto kOxidizerMainValveID = 60;
  static constexpr auto kOxidizerVentValveID = 61;
  static constexpr auto kOxidizerDumpValveID = 62;
  static constexpr auto kPressureFeedSystemMainValveID  = 63;
  static constexpr auto kPressureFeedSystemVentValveID  = 64;

  static constexpr auto kDiagMainValveInstance = "/srp/apps/servoService/MainServoStatus";
  static constexpr auto kDiagVentValveInstance = "/srp/apps/servoService/VentServoStatus";
  static constexpr auto kDiagDumpValveInstance = "/srp/apps/servoService/DumpServoStatus";

  static constexpr auto kIpcInstanceSpecifier = "srp/apps/servoService/ServoService_ipc";
  static constexpr auto kUdpInstanceSpecifier = "srp/apps/servoService/ServoService_udp";
  static constexpr auto kHeartBeatPinID = 4;
}  // namespace

ServoService::ServoService() {}

int ServoService::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "ServoService.Run: offering services";
  if (!servo_controller || !service_ipc || !service_udp) {
    ara::log::LogError() << "ServoService.Run: service components are not initialized";
    return 1;
  }

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
            ara::log::LogInfo() << "ServoService.Run: " << name
                                << " servo status changed to " << val.value();
            last_value = val.value();
          }
      } else {
          ara::log::LogWarn() << "ServoService.Run: failed to read " << name
                              << " servo status";
      }
  };

  std::optional<uint8_t> last_main_state;
  std::optional<uint8_t> last_vent_state;
  std::optional<uint8_t> last_dump_state;
  std::optional<uint8_t> last_eth_main_state;
  std::optional<uint8_t> last_eth_vent_state;


  while (!token.stop_requested()) {
    if (gpio_.SetPinValue(kHeartBeatPinID, 1, 500) != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "ServoService::Run: failed to toggle heartbeat pin";
    }
    update_servo_status(kOxidizerMainValveID, service_ipc->newOxidizerMainValveEvent, service_udp->newOxidizerMainValveEvent,
                        "oxi_main", last_main_state);
    update_servo_status(kOxidizerVentValveID, service_ipc->newOxidizerVentValveEvent, service_udp->newOxidizerVentValveEvent,
                        "oxi_vent", last_vent_state);
    update_servo_status(kOxidizerDumpValveID, service_ipc->newOxidizerDumpValveEvent, service_udp->newOxidizerDumpValveEvent,
                        "oxi_dump", last_dump_state);
    update_servo_status(kPressureFeedSystemMainValveID, service_ipc->newPressureFeedMainEvent, service_udp->newPressureFeedMainEvent,
                        "eth_main", last_eth_main_state);
    update_servo_status(kPressureFeedSystemVentValveID, service_ipc->newPressureFeedVentEvent, service_udp->newPressureFeedVentEvent,
                        "eth_dump", last_eth_vent_state);

    core::condition::wait_for(kEventIntervalMs, token);
  }

  ara::log::LogInfo() << "ServoService.Run: stopping offers";

  service_ipc->StopOffer();
  service_udp->StopOffer();

  ara::log::LogInfo() << "ServoService.Run: stopped";
  return 0;
}

int ServoService::Initialize(const std::map<ara::core::StringView, ara::core::StringView> parms) {
  ara::log::LogInfo() << "ServoService.Initialize: start";

  if (parms.find("app_path") == parms.end()) {
      ara::log::LogError() << "ServoService.Initialize: missing 'app_path' parameter";
      return 1;
  }

  this->servo_controller = std::make_shared<ServoController>();
  std::string app_path(parms.at("app_path"));

  ara::log::LogDebug() << "ServoService.Initialize: using app path " << app_path;

  this->servo_controller->Init(app_path);

  service_ipc = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier(kIpcInstanceSpecifier), this->servo_controller);

  service_udp = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier(kUdpInstanceSpecifier), this->servo_controller);

  ara::log::LogInfo() << "ServoService.Initialize: completed";
  return 0;
}

}  // namespace service
}  // namespace srp
