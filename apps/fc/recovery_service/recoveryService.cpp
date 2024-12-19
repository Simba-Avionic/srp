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
#include "apps/fc/recovery_service/recoveryService.hpp"

#include <memory>
#include <optional>
#include <vector>

#include "ara/log/log.h"
#include "core/common/condition.h"
namespace simba {
namespace service {

int RecoveryService::Run(const std::stop_token& token) {
  while (!token.stop_requested()) {
    ara::log::LogDebug() << ("Send servo status event");
    core::condition::wait_for(std::chrono::milliseconds(500), token);
  }
  return 0;
}
RecoveryService::RecoveryService() : service_ipc_instance("simba/apps/servoService/ServoService_ipc"),
    service_udp_instance("simba/apps/servoService/ServoService_udp") {
}

int RecoveryService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  this->servo_controller = std::make_shared<i2c::PCA9685>();
  this->gpio_controller = std::make_shared<gpio::GPIOController>();
  this->servo_controller->Init(parms.at("app_path"), std::make_unique<simba::i2c::I2CController>(),
                              std::make_unique<gpio::GPIOController>());
  service_ipc = std::make_unique<apps::MyRecoveryServiceSkeleton>(this->service_ipc_instance, this->servo_controller,
                                                        this->gpio_controller);
  service_udp = std::make_unique<apps::MyRecoveryServiceSkeleton>(this->service_udp_instance, this->servo_controller,
                                                        this->gpio_controller);
  return 0;
}

}  // namespace service
}  // namespace simba
