#include "blinking_service.hpp"

#include "ara/log/log.h"
#include "core/common/condition.h"
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <utility>

namespace simba {
namespace envService {

void BlinkingService::setDiodeState(const uint8_t state) {
  _gpio.SetPinValue(DIODE_PIN, state);
}

void BlinkingService::blinkingLoop(const std::stop_token &token) {
  uint8_t state = 0;
  while (!token.stop_requested()) {
    state = 1 - state;
    std::cout << "setting state: " << (int)state << std::endl;
    setDiodeState(state);
    core::condition::wait_for(std::chrono::milliseconds(100), token);
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
    : _gpio(std::make_unique<com::soc::StreamIpcSocket>()) {}

} // namespace envService
} // namespace simba
