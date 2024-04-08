/**
 * @file dtc_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "diag/dtc/controller/dtc_controller.h"

#include <vector>

#include "core/logger/Logger.h"
#include "diag/dtc/data/dtc_header.hpp"
namespace simba {
namespace diag {
namespace dtc {

void DtcController::Init(uint16_t service_id) {
  //   this->sock_.Init(*config.get());
  this->service_id = service_id;
}

core::ErrorCode DtcController::RegisterDTC(std::weak_ptr<IDTC> dtc_object) {
  dtc_object.lock()->SetCallback(std::bind(&DtcController::RXCallback, this,
                                           std::placeholders::_1,
                                           std::placeholders::_2));
  AppLogger::Info("[DTC]: New DTC added: " +
                  std::to_string(dtc_object.lock()->GetId()));
  return core::ErrorCode::kOk;
}

void DtcController::RXCallback(const uint8_t& dtc_id, const uint8_t& flag) {
  AppLogger::Info("[DTC]: State changed for " + std::to_string(dtc_id) +
                  " new status " + std::to_string(flag));
}

}  // namespace dtc
}  // namespace diag
}  // namespace simba
