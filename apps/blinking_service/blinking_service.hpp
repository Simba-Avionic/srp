/* Copyright © 2025 Karol Zwolak <karolzwolak@gmail.com> */
#ifndef APPS_BLINKING_SERVICE_BLINKING_SERVICE_HPP_
#define APPS_BLINKING_SERVICE_BLINKING_SERVICE_HPP_

#include <chrono>  // NOLINT
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ara/exec/adaptive_application.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"

namespace simba {
namespace envService {

class BlinkingService final : public ara::exec::AdaptiveApplication {
 private:
  std::vector<uint8_t> diode_pins;
  std::vector<std::chrono::milliseconds> diode_delays;
  std::vector<uint8_t> diode_states;
  std::vector<std::chrono::_V2::system_clock::time_point> diode_next_toggle;

  gpio::GPIOController _gpio;

  void setDiodeState(const uint8_t diode_pin_id, const uint8_t state);
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

}  // namespace envService
}  // namespace simba

#endif  // APPS_BLINKING_SERVICE_BLINKING_SERVICE_HPP_
