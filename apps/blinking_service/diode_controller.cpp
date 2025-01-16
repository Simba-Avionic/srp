/* Copyright © 2025 Karol Zwolak <karolzwolak@gmail.com> */

#include "diode_controller.hpp"

#include <iostream>

namespace simba {
namespace blinkingService {

DiodeState DiodeController::readDiodeStateById(const int diode_id) {
  return diode_states[diode_id];
}

DiodeState DiodeController::readDiodeStateByPin(const DiodePin diode_pin) {
  auto diode_id = diode_pin_to_index[diode_pin];
  return DiodeController::readDiodeStateById(diode_id);
}

void DiodeController::setDiodeStateById(const int diode_id,
                                        const DiodeState state) {
  return DiodeController::setDiodeStateByPin(diode_pins[diode_id], state);
}
void DiodeController::setDiodeStateByPin(const DiodePin diode_pin,
                                         const DiodeState state) {
  int diode_id = diode_pin_to_index[diode_pin];
  std::cout << "setting diode with pin#" << static_cast<int>(diode_pin)
            << " to state " << static_cast<int>(state) << std::endl;
  diode_states[diode_id] = state;
  gpio_controller.SetPinValue(diode_pin, state);
}

void DiodeController::initializeVectors() {
  for (size_t i = 0; i < diode_pins.size(); i++) {
    diode_pin_to_index[diode_pins[i]] = i;
    diode_states.push_back(0);
  }
}

DiodeController::DiodeController(std::vector<DiodePin> diode_pins)
    : gpio_controller(std::make_unique<com::soc::StreamIpcSocket>()),
      diode_pins(diode_pins),
      diode_pin_to_index() {
  initializeVectors();
}

}  // namespace blinkingService
}  // namespace simba
