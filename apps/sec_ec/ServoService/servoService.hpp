/**
 * @file servoService.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Secondary EC Servo Service application
 * @version 0.1
 * @date 2026-07-01
 *
 * @copyright Copyright (c) 2026
 */
#ifndef APPS_SEC_EC_SERVOSERVICE_SERVOSERVICE_HPP_
#define APPS_SEC_EC_SERVOSERVICE_SERVOSERVICE_HPP_

#include <string>
#include <memory>
#include <map>

#include "ara/exec/adaptive_application.h"

#include "apps/ec/ServoService/servoController/servo_controller.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "apps/sec_ec/ServoService/service.hpp"

namespace srp {
namespace sec_ec {

class SecServoService final : public ara::exec::AdaptiveApplication {
 private:
  std::shared_ptr<service::ServoController> servo_controller;
  std::unique_ptr<apps::MySecServoService> service_ipc;
  std::unique_ptr<apps::MySecServoService> service_udp;
  gpio::GPIOController gpio_;

 protected:
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;
  int Run(const std::stop_token& token) override;

 public:
  ~SecServoService() = default;
  SecServoService();
};

}  // namespace sec_ec
}  // namespace srp

#endif  // APPS_SEC_EC_SERVOSERVICE_SERVOSERVICE_HPP_
