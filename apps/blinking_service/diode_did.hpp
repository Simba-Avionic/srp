/* Copyright © 2025 Karol Zwolak <karolzwolak@gmail.com> */
#ifndef APPS_BLINKING_SERVICE_DIODE_DID_HPP_
#define APPS_BLINKING_SERVICE_DIODE_DID_HPP_

#include <memory>

#include "diag/jobs/skeleton/did_job.h"
#include "diode_controller.hpp"

namespace simba {
namespace blinkingService {

class DiodeDiD : public diag::DiDJob {
 private:
  std::shared_ptr<simba::blinkingService::DiodeController> diode_controller;
  DiodePin diode_pin;

  diag::DiagResponse Read();
  diag::DiagResponse Write(const std::vector<uint8_t>& payload);

 public:
  DiodeDiD(
      const ara::core::InstanceSpecifier& instance,
      std::shared_ptr<simba::blinkingService::DiodeController> diode_controller,
      DiodePin diode_pin);
  ~DiodeDiD() = default;
};

}  // namespace blinkingService
}  // namespace simba

#endif  // APPS_BLINKING_SERVICE_DIODE_DID_HPP_
