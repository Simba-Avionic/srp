/**
 * @file servo_state.hpp
 * @brief Stan zaworu (serwa) ustawiany przez SOME/IP / DID: 0 zamkniety, 1 otwarty, 2 otwarty na stale
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_STATE_HPP_
#define APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_STATE_HPP_
#include <cstdint>
#include <optional>

namespace srp {
namespace service {

enum class ServoState : uint8_t {
  kClosed = 0,
  /// otwarty; podlega auto-zamykaniu (`auto_closing`) i pulsowaniu (`pulsing_time`) z configu
  kOpen = 1,
  /// otwarty na stale: bez pulsowania i bez auto-zamykania, do jawnego 0
  kOpenPermanent = 2,
};

constexpr uint8_t ToRaw(ServoState s) noexcept { return static_cast<uint8_t>(s); }

/// Walidacja wartosci z SOME/IP / DID; nullopt dla wartosci spoza 0..2.
constexpr std::optional<ServoState> ToServoState(uint8_t v) noexcept {
  if (v > ToRaw(ServoState::kOpenPermanent)) {
    return std::nullopt;
  }
  return static_cast<ServoState>(v);
}

}  // namespace service
}  // namespace srp

#endif  // APPS_EC_SERVOSERVICE_SERVOCONTROLLER_SERVO_STATE_HPP_
