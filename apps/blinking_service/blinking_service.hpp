
#ifndef APPS_TEST_TEST_HPP_
#define APPS_TEST_TEST_HPP_

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
  static constexpr uint8_t DIODE_PIN = 1;
  gpio::GPIOController _gpio;

  void setDiodeState(const uint8_t state);
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

} // namespace envService
} // namespace simba

#endif
