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
#include <vector>

#include "ara/log/log.h"
#include "core/common/condition.h"
namespace srp {
namespace service {

namespace {
  constexpr auto kDiag_main_instance_path = "/srp/apps/servoService/MainServoStatus";
  constexpr auto kDiag_venv_instance_path = "/srp/apps/servoService/VentServoStatus";
  constexpr auto kDiag_serv_instance_path = "/srp/apps/servoService/ServoDID";
  constexpr auto kservice_ipc_path = "srp/apps/servoService/ServoService_ipc";
  constexpr auto kservice_udp_path = "srp/apps/servoService/ServoService_udp";
}

ServoService::ServoService(): diag_main_instance(kDiag_main_instance_path),
                diag_venv_instance(kDiag_venv_instance_path),
                diag_serv_instance(kDiag_serv_instance_path) {
}

int ServoService::Run(const std::stop_token& token) {
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
                              std::make_unique<srp::i2c::I2CController>(),
                              std::make_unique<gpio::GPIOController>());
  main_servo_service_did_ = std::make_unique<ServoServiceDiD>(diag_main_instance, servo_controller, 60);
  vent_servo_service_did_ = std::make_unique<ServoServiceDiD>(diag_venv_instance, servo_controller, 61);
  servo_did_ = std::make_unique<ServoSecondDid>(diag_serv_instance, this->servo_controller);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  // TODO(matikrajek42@gmail.com) remove after fix someip timeout error
  service_ipc = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier(kservice_ipc_path), this->servo_controller);
  service_udp = std::make_unique<apps::MyServoService>(
                ara::core::InstanceSpecifier(kservice_udp_path), this->servo_controller);
  return 0;
}

}  // namespace service
}  // namespace srp
