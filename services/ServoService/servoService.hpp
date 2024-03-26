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
#ifndef SERVICES_SERVOSERVICE_SERVOSERVICE_HPP_
#define SERVICES_SERVOSERVICE_SERVOSERVICE_HPP_
#include <string>
#include <unordered_map>
#include <memory>

#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "communication-core/someip-controller/event_skeleton.h"
#include "communication-core/someip-controller/method_proxy.h"

#include "core/application/application_no_ipc.h"
namespace simba {
namespace service {
class ServoService : public core::ApplicationNoIPC{
 private:
  std::shared_ptr<simba::com::someip::EventSkeleton> event_servo_status;
  std::shared_ptr<simba::com::someip::EventSkeleton> event_vent_status;
 protected:
  core::ErrorCode Run(std::stop_token token) final;

  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;

 public:
  ~ServoService() = default;
};

}  // namespace service
}  // namespace simba
#endif  // SERVICES_SERVOSERVICE_SERVOSERVICE_HPP_
