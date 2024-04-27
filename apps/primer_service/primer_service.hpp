/**
 * @file primer_service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_PRIMER_SERVICE_PRIMER_SERVICE_HPP_
#define APPS_PRIMER_SERVICE_PRIMER_SERVICE_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "core/application/application_no_ipc.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "communication-core/someip-controller/event_skeleton.h"
#include "diag/dtc/controller/dtc.h"
namespace simba {
namespace primer {
struct Primer_t {
  uint8_t actuator_id;
  gpio::Value state{gpio::Value::LOW};
};

class PrimerService final : public core::ApplicationNoIPC {
 private:
  core::ErrorCode ChangePrimerState(gpio::Value state);
  core::ErrorCode ReadConfig(const std::unordered_map<std::string, std::string>& parms);

  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  core::ErrorCode Run(std::stop_token token) final;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with parms
   */
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;

  gpio::GPIOController gpio_;
  gpio::Value primerState {gpio::Value::LOW};
  uint8_t primer_pin_;
  std::uint16_t active_time;

  std::shared_ptr<simba::com::someip::EventSkeleton> primer_event;

  std::shared_ptr<simba::diag::dtc::DTCObject> dtc_30;
  std::shared_ptr<simba::diag::dtc::DTCObject> dtc_31;
};

}  // namespace primer
}  // namespace simba
#endif  // APPS_PRIMER_SERVICE_PRIMER_SERVICE_HPP_
