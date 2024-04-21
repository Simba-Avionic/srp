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
#include <unordered_map>

#include "core/application/application_no_ipc.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
namespace simba {
namespace primer {
struct Primer_t {
  uint8_t actuator_id;
  gpio::Value state{gpio::Value::LOW};
};

class PrimerService final : public core::ApplicationNoIPC {
 private:
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
};

}  // namespace primer
}  // namespace simba
#endif  // APPS_PRIMER_SERVICE_PRIMER_SERVICE_HPP_
