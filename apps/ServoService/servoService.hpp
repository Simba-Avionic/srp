/**
 * @file servoService.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_SERVOSERVICE_SERVOSERVICE_HPP_
#define APPS_SERVOSERVICE_SERVOSERVICE_HPP_
#include <string>
#include <unordered_map>
#include <memory>

#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "communication-core/someip-controller/event_skeleton.h"
#include "communication-core/someip-controller/method_proxy.h"
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "core/application/application_no_ipc.h"
namespace simba {
namespace service {
class ServoService : public core::ApplicationNoIPC{
 private:
  std::shared_ptr<simba::com::someip::EventSkeleton> main_servo_status_event;
  std::shared_ptr<simba::com::someip::EventSkeleton> vent_servo_status_event;
  std::shared_ptr<simba::com::someip::MethodSkeleton> set_servo_val;
  std::shared_ptr<simba::com::someip::MethodSkeleton> set_vent_val;
  i2c::PCA9685 servo_controller;
 protected:
  core::ErrorCode Run(const std::stop_token& token) final;

  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;

 public:
  ~ServoService() = default;
};

}  // namespace service
}  // namespace simba
#endif  // APPS_SERVOSERVICE_SERVOSERVICE_HPP_
