/* Copyright © 2025 Karol Zwolak <karolzwolak@gmail.com> */
#include "blinking_service.hpp"

#include <algorithm>
#include <chrono>  // NOLINT
#include <iostream>
#include <map>
#include <memory>

#include "ara/log/log.h"
#include "core/common/condition.h"
#include "diode_controller.hpp"

namespace simba {
namespace blinkingService {

void BlinkingService::blinkingLoop(const std::stop_token &token) {
  auto start = std::chrono::high_resolution_clock::now();
  while (!token.stop_requested()) {
    auto min_ms_to_next_toggle = diode_delays[0];
    auto now = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < diode_delays.size(); i++) {
      auto elapsed_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(start - now);
      if (now >= diode_next_toggle[i]) {
        DiodeState diode_state = diode_controller->readDiodeStateById(i);
        diode_controller->setDiodeStateById(i, 1 - diode_state);
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
  diode_did->StartOffer();
  blinkingLoop(token);
  diode_did->StopOffer();
  return 0;
}
BlinkingService::BlinkingService()
    : diode_delays(
          {std::chrono::milliseconds(100), std::chrono::milliseconds(333)}),
      diode_next_toggle(diode_delays.size(),
                        std::chrono::high_resolution_clock::now()),
      diode_controller(
          std::make_shared<simba::blinkingService::DiodeController>(
              std::vector<DiodePin>({1, 2, DID_CONTROLLED_PIN}))),
      diode_did(std::make_unique<DiodeDiD>(
          ara::core::InstanceSpecifier("/simba/apps/blinkingService/DiodeDiD"),
          diode_controller, DID_CONTROLLED_PIN)) {}

}  // namespace blinkingService
}  // namespace simba
