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
#include "servoService.hpp"

#include <memory>
#include <optional>
#include <vector>

#include "ara/log/log.h"
#include "core/common/condition.h"
namespace simba {
namespace service {

int ServoService::Run(const std::stop_token& token) {
  main_servo_service_did_->StartOffer();
  vent_servo_service_did_->StartOffer();

  service_ipc->StartOffer();
  service_udp->StartOffer();

  while (!token.stop_requested()) {
    // update servo positions;
    auto val = this->servo_controller->ReadServoPosition(60);
    if (val.has_value()) {
      service_ipc->ServoStatusEvent.Update(val.value());
      service_udp->ServoStatusEvent.Update(val.value());
    }
    auto val2 = this->servo_controller->ReadServoPosition(61);
    if (val2.has_value()) {
      service_ipc->ServoVentStatusEvent.Update(val2.value());
      service_udp->ServoVentStatusEvent.Update(val2.value());
    }
    ara::log::LogDebug() << ("Send servo status event");
    core::condition::wait_for(std::chrono::milliseconds(500), token);
  }
  service_ipc->StopOffer();
  service_udp->StopOffer();

  main_servo_service_did_->StopOffer();
  vent_servo_service_did_->StopOffer();
  return 0;
}

int ServoService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  this->servo_controller = std::make_shared<i2c::PCA9685>();
  this->servo_controller->Init(parms.at("app_path"),
                              std::make_unique<simba::i2c::I2CController>(),
                              std::make_unique<gpio::GPIOController>());
  main_servo_service_did_ = std::make_unique<ServoServiceDiD>(
                ara::core::InstanceSpecifier("/simba/apps/servoService/MainServoStatus"), servo_controller, 60);
  vent_servo_service_did_ = std::make_unique<ServoServiceDiD>(
                ara::core::InstanceSpecifier("/simba/apps/servoService/VentServoStatus"), servo_controller, 61);
  service_ipc = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier("simba/apps/servoService/ServoService_ipc"), this->servo_controller);
  service_udp = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier("simba/apps/servoService/ServoService_udp"), this->servo_controller);
  return 0;
}

}  // namespace service
}  // namespace simba
