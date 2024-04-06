/**
 * @file I_dtc_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DIAG_DTC_CONTROLLER_I_DTC_CONTROLLER_H_
#define DIAG_DTC_CONTROLLER_I_DTC_CONTROLLER_H_

#include <memory>
#include <vector>

#include "communication-core/sockets/socket_config.h"
#include "core/common/error_code.h"
#include "diag/dtc/controller/i_dtc.h"

namespace simba {
namespace diag {
namespace dtc {

class IDtcController {
 public:
  virtual void Init(uint16_t service_id) = 0;
  virtual core::ErrorCode RegisterDTC(std::weak_ptr<IDTC> dtc_object) = 0;
};

}  // namespace dtc
}  // namespace diag
}  // namespace simba

#endif  // DIAG_DTC_CONTROLLER_I_DTC_CONTROLLER_H_
