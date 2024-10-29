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
#include <map>
#include <vector>
#include <unordered_map>
#include <memory>

#include "ara/exec/adaptive_application.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
namespace simba {
namespace primer {

class PrimerService final :  public ara::exec::AdaptiveApplication {
 private:
  core::ErrorCode ChangePrimerState(uint8_t state);
  core::ErrorCode ReadConfig(const std::map<ara::core::StringView, ara::core::StringView>
                      parms);

  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;

  gpio::GPIOController gpio_;
  uint8_t primerState;
  std::vector<uint8_t> primer_pins_;
  std::uint16_t active_time;
 public:
  ~PrimerService() = default;

  // std::shared_ptr<simba::com::someip::EventSkeleton> primer_event;

  // std::shared_ptr<simba::diag::dtc::DTCObject> dtc_30;
  // std::shared_ptr<simba::diag::dtc::DTCObject> dtc_31;
};

}  // namespace primer
}  // namespace simba
#endif  // APPS_PRIMER_SERVICE_PRIMER_SERVICE_HPP_
