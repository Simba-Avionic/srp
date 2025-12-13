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
  constexpr auto kEventIntervalMs = std::chrono::milliseconds(1000);

  constexpr std::uint8_t kMainValveID = 60;
  constexpr std::uint8_t kVentValveID = 61;
  constexpr std::uint8_t kDumpValveID = 62;

  constexpr auto kDiagMainValveInstance = "/srp/apps/servoService/MainServoStatus";
  constexpr auto kDiagVentValveInstance = "/srp/apps/servoService/VentServoStatus";
  constexpr auto kDiagDumpValveInstance = "/srp/apps/servoService/DumpServoStatus";
  constexpr auto kDiagServeInstance = "/srp/apps/servoService/ServoDID";

  constexpr auto kIpcInstanceSpecifier = "srp/apps/servoService/ServoService_ipc";
  constexpr auto kUdpInstanceSpecifier = "srp/apps/servoService/ServoService_udp";
}  // namespace

ServoService::ServoService():
    diag_main_instance(kDiagMainValveInstance),
    diag_venv_instance(kDiagVentValveInstance),
    diag_dump_instance(kDiagDumpValveInstance),
    diag_serv_instance{kDiagServeInstance} {
}

int ServoService::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "ServoService.Run: offering services";

  main_servo_service_did_->Offer();
  vent_servo_service_did_->Offer();
  dump_servo_service_did_->Offer();
  servo_did_->Offer();
  service_ipc->StartOffer();
  service_udp->StartOffer();

  auto update_servo_status = [&](std::uint8_t id, auto& ipc_event, auto& udp_event, const char* name) {
      auto val = this->servo_controller->ReadServoPosition(id);
      if (val.has_value()) {
          ipc_event.Update(val.value());
          udp_event.Update(val.value());
          ara::log::LogDebug() << "ServoService.Run: updated " << name <<
                    " servo status: " << std::to_string(static_cast<int>(val.value()));
      }
  };

  while (!token.stop_requested()) {
    update_servo_status(kMainValveID, service_ipc->ServoStatusEvent,     service_udp->ServoStatusEvent,     "main");
    update_servo_status(kVentValveID, service_ipc->ServoVentStatusEvent, service_udp->ServoVentStatusEvent, "vent");
    update_servo_status(kDumpValveID, service_ipc->ServoDumpStatusEvent, service_udp->ServoDumpStatusEvent, "dump");

    ara::log::LogDebug() << "ServoService.Run: status events check cycle completed";
    core::condition::wait_for(kEventIntervalMs, token);
  }

  ara::log::LogInfo() << "ServoService.Run: stopping offers";

  service_ipc->StopOffer();
  service_udp->StopOffer();
  servo_did_->StopOffer();
  dump_servo_service_did_->StopOffer();
  vent_servo_service_did_->StopOffer();
  main_servo_service_did_->StopOffer();

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

  auto err = this->servo_controller->Init(
      app_path, std::make_shared<srp::i2c::PCA9685>(),
      std::make_unique<gpio::GPIOController>(), std::make_unique<srp::i2c::I2CController>());

  if (err != core::ErrorCode::kOk) {
    ara::log::LogError() << "ServoService.Initialize: cannot initialize servo controller";
    return 1;
  }

  main_servo_service_did_ = std::make_unique<ServoServiceDiD>(diag_main_instance, servo_controller, kMainValveID);
  vent_servo_service_did_ = std::make_unique<ServoServiceDiD>(diag_venv_instance, servo_controller, kVentValveID);
  dump_servo_service_did_ = std::make_unique<ServoServiceDiD>(diag_dump_instance, servo_controller, kDumpValveID);
  servo_did_ = std::make_unique<ServoSecondDid>(diag_serv_instance, this->servo_controller);

  service_ipc = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier(kIpcInstanceSpecifier), this->servo_controller);

  service_udp = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier(kUdpInstanceSpecifier), this->servo_controller);

  ara::log::LogInfo() << "ServoService.Initialize: completed";
  return 0;
}

}  // namespace service
}  // namespace srp
