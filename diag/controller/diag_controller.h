/**
 * @file diag_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DIAG_CONTROLLER_DIAG_CONTROLLER_H_
#define DIAG_CONTROLLER_DIAG_CONTROLLER_H_

#include <memory>

#include "diag/dtc/controller/dtc_controller.h"
#include "diag/dtc/controller/i_dtc.h"

namespace simba {
namespace diag {

class CommonDiagController {
 public:
  core::ErrorCode RegisterDTC(std::weak_ptr<dtc::IDTC> dtc_object) {
    this->dtc_controller.RegisterDTC(dtc_object);
    return core::ErrorCode::kOk;
  }
  void Init(uint16_t service_id) { dtc_controller.Init(service_id); }

 private:
  dtc::DtcController dtc_controller{};
};
}  // namespace diag
}  // namespace simba

#endif  // DIAG_CONTROLLER_DIAG_CONTROLLER_H_
