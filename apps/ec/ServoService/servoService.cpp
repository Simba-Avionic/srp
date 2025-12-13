/**
 * @file servoService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
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
  constexpr auto kEvent_interval_ms = 1000;
}

ServoService::ServoService():
                diag_main_instance("/srp/apps/servoService/MainServoStatus"),
                diag_venv_instance("/srp/apps/servoService/VentServoStatus"),
                diag_serv_instance{"/srp/apps/servoService/ServoDID"} {
}

int ServoService::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "ServoService.Run: offering services";
  main_servo_service_did_->Offer();
  vent_servo_service_did_->Offer();
  servo_did_->Offer();
  service_ipc->StartOffer();
  service_udp->StartOffer();
  while (!token.stop_requested()) {
    // update servo positions;
    auto val = this->servo_controller->ReadServoPosition(60);
    if (val.has_value()) {
      service_ipc->ServoStatusEvent.Update(val.value());
      service_udp->ServoStatusEvent.Update(val.value());
      ara::log::LogDebug() << "ServoService.Run: updated main servo status " << std::to_string(val.value());
    }
    auto val2 = this->servo_controller->ReadServoPosition(61);
    if (val2.has_value()) {
      service_ipc->ServoVentStatusEvent.Update(val2.value());
      service_udp->ServoVentStatusEvent.Update(val2.value());
      ara::log::LogDebug() << "ServoService.Run: updated vent servo status " << std::to_string(val2.value());
    }
    ara::log::LogDebug() << "ServoService.Run: status events sent";
    core::condition::wait_for(std::chrono::milliseconds(kEvent_interval_ms), token);
  }
  ara::log::LogInfo() << "ServoService.Run: stopping offers";
  service_ipc->StopOffer();
  service_udp->StopOffer();

  main_servo_service_did_->StopOffer();
  vent_servo_service_did_->StopOffer();
  servo_did_->StopOffer();
  ara::log::LogInfo() << "ServoService.Run: stopped";
  return 0;
}

int ServoService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  ara::log::LogInfo() << "ServoService.Initialize: start";
  this->servo_controller = std::make_shared<ServoController>();
  auto app_path = std::string(parms.at("app_path"));
  ara::log::LogDebug() << "ServoService.Initialize: using app path " << app_path;
  auto err = this->servo_controller->Init(
      app_path,
      std::make_shared<srp::i2c::PCA9685>(),
      std::make_unique<gpio::GPIOController>(),
      std::make_unique<srp::i2c::I2CController>());
  if (err != core::ErrorCode::kOk) {
    ara::log::LogError() << "ServoService.Initialize: cannot initialize servo controller";
    return 1;
  }
  main_servo_service_did_ = std::make_unique<ServoServiceDiD>(diag_main_instance, servo_controller, 60);
  vent_servo_service_did_ = std::make_unique<ServoServiceDiD>(diag_venv_instance, servo_controller, 61);
  servo_did_ = std::make_unique<ServoSecondDid>(diag_serv_instance, this->servo_controller);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  // TODO(matikrajek42@gmail.com) remove after fix someip timeout error
  service_ipc = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier("srp/apps/servoService/ServoService_ipc"), this->servo_controller);
  service_udp = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier("srp/apps/servoService/ServoService_udp"), this->servo_controller);
  ara::log::LogInfo() << "ServoService.Initialize: completed";
  return 0;
}

}  // namespace service
}  // namespace srp
