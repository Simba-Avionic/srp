/**
 * @file primer_service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef SERVICES_PRIMER_SERVICE_PRIMER_SERVICE_HPP_
#define SERVICES_PRIMER_SERVICE_PRIMER_SERVICE_HPP_

#include <string>
#include <unordered_map>

#include "core/application/application_no_ipc.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
namespace simba {
namespace primer {
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
 public:
  PrimerService(/* args */);
  ~PrimerService();
};

}  // namespace primer
}  // namespace simba
#endif  // SERVICES_PRIMER_SERVICE_PRIMER_SERVICE_HPP_
