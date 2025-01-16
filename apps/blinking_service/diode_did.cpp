/* Copyright © 2025 Karol Zwolak <karolzwolak@gmail.com> */
#include "diode_did.hpp"

#include "diode_controller.hpp"

namespace simba {
namespace blinkingService {

diag::DiagResponse DiodeDiD::Read() {
  return diag::DiagResponse(
      diag::DiagResponseCodes::kOk,
      {diode_controller->readDiodeStateByPin(DID_CONTROLLED_PIN)});
}

diag::DiagResponse DiodeDiD::Write(const std::vector<uint8_t>& payload) {
  if (payload.size() != 1) {
    return diag::DiagResponse{
        diag::DiagResponseCodes::kInvalidMessageLengthFormat};
  }
  diode_controller->setDiodeStateByPin(DID_CONTROLLED_PIN, payload[0]);

  return diag::DiagResponse{diag::DiagResponseCodes::kOk};
}

DiodeDiD::DiodeDiD(
    const ara::core::InstanceSpecifier& instance,
    std::shared_ptr<simba::blinkingService::DiodeController> diode_controller,
    DiodePin diode_pin)
    : diag::DiDJob(instance),
      diode_controller(diode_controller),
      diode_pin(diode_pin) {}

}  // namespace blinkingService
}  // namespace simba
