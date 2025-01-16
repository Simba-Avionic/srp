/* Copyright © 2025 Karol Zwolak <karolzwolak@gmail.com> */
#ifndef APPS_BLINKING_SERVICE_DIODE_CONTROLLER_HPP_
#define APPS_BLINKING_SERVICE_DIODE_CONTROLLER_HPP_

#include <cstdint>
#include <map>
#include <vector>

#include "mw/gpio_server/controller/gpio_controller.hpp"

typedef uint8_t DiodePin;
typedef uint8_t DiodeState;

constexpr DiodePin DID_CONTROLLED_PIN = 3;

namespace simba {
namespace blinkingService {

class DiodeController final {
 private:
  std::vector<DiodePin> diode_pins;
  std::vector<DiodeState> diode_states;
  std::map<DiodePin, int> diode_pin_to_index;

  gpio::GPIOController gpio_controller;

  void initializeVectors();

 public:
  DiodeState readDiodeStateByPin(const DiodePin diode_pin_id);
  DiodeState readDiodeStateById(const int diode_id);
  void setDiodeStateByPin(const DiodePin diode_pin, const DiodeState state);
  void setDiodeStateById(const int diode_id, const DiodeState state);

  ~DiodeController() = default;
  DiodeController(std::vector<DiodePin> diode_pins);
};

}  // namespace blinkingService
}  // namespace simba

#endif  // APPS_BLINKING_SERVICE_DIODE_CONTROLLER_HPP_
