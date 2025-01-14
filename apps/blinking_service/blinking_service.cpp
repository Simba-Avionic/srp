#include "blinking_service.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <utility>

#include "ara/log/log.h"
#include "core/common/condition.h"

namespace simba {
namespace envService {

void BlinkingService::setDiodeState(const uint8_t diode_pin_id,
                                    const uint8_t state) {
  std::cout << "setting diode with pin #" << static_cast<int>(diode_pin_id)
            << " to state " << static_cast<int>(state) << std::endl;
  _gpio.SetPinValue(diode_pin_id, state);
}

void BlinkingService::blinkingLoop(const std::stop_token &token) {
  auto start = std::chrono::high_resolution_clock::now();
  while (!token.stop_requested()) {
    auto min_ms_to_next_toggle = diode_delays[0];
    auto now = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < diode_pins.size(); i++) {
      auto elapsed_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(start - now);
      if (now >= diode_next_toggle[i]) {
        diode_states[i] = 1 - diode_states[i];
        setDiodeState(diode_pins[i], diode_states[i]);
        diode_next_toggle[i] = now + diode_delays[i];
      }
      auto ms_next_toggle =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              diode_next_toggle[i] - now);
      min_ms_to_next_toggle = min(min_ms_to_next_toggle, ms_next_toggle);
    }
    if (min_ms_to_next_toggle < std::chrono::milliseconds(1)) continue;
    std::cout << "sleeping for " << min_ms_to_next_toggle << std::endl;
    core::condition::wait_for(min_ms_to_next_toggle, token);
  }
}

int BlinkingService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  return 0;
}

int BlinkingService::Run(const std::stop_token &token) {
  blinkingLoop(token);
  return 0;
}
BlinkingService::BlinkingService()
    : diode_pins({1, 2}),
      diode_delays(
          {std::chrono::milliseconds(100), std::chrono::milliseconds(333)}),
      diode_states(diode_pins.size(), 0),
      diode_next_toggle(diode_pins.size(),
                        std::chrono::high_resolution_clock::now()),
      _gpio(std::make_unique<com::soc::StreamIpcSocket>()) {}

}  // namespace envService
}  // namespace simba
