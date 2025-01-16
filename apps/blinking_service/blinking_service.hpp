/* Copyright © 2025 Karol Zwolak <karolzwolak@gmail.com> */
#ifndef APPS_BLINKING_SERVICE_BLINKING_SERVICE_HPP_
#define APPS_BLINKING_SERVICE_BLINKING_SERVICE_HPP_

#include <chrono>  // NOLINT
#include <map>     // not actually used here, but required to pass cpplint
#include <memory>
#include <vector>

#include "ara/exec/adaptive_application.h"
#include "diode_controller.hpp"
#include "diode_did.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"

namespace simba {
namespace blinkingService {

class BlinkingService final : public ara::exec::AdaptiveApplication {
 private:
  std::vector<std::chrono::milliseconds> diode_delays;
  std::vector<std::chrono::_V2::system_clock::time_point> diode_next_toggle;

  std::shared_ptr<simba::blinkingService::DiodeController> diode_controller;
  std::unique_ptr<DiodeDiD> diode_did;

  void blinkingLoop(const std::stop_token &token);

 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token &token) override;
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                     parms) override;

 public:
  ~BlinkingService() = default;
  BlinkingService();
};

}  // namespace blinkingService
}  // namespace simba

#endif  // APPS_BLINKING_SERVICE_BLINKING_SERVICE_HPP_
