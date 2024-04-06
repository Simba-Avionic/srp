/**
 * @file dtc_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DIAG_DTC_CONTROLLER_DTC_CONTROLLER_H_
#define DIAG_DTC_CONTROLLER_DTC_CONTROLLER_H_

#include <memory>
#include <vector>

#include "communication-core/sockets/ipc_socket.h"
#include "diag/dtc/controller/I_dtc_controller.h"
#include "diag/dtc/factories/dtc_msg_factory.hpp"

namespace simba {
namespace diag {
namespace dtc {

class DtcController : public IDtcController {
 public:
  core::ErrorCode RegisterDTC(std::weak_ptr<IDTC> dtc_object) override;
  void Init(uint16_t service_id) override;

 private:
  void RXCallback(const uint8_t& dtc_id, const uint8_t& flag);
  com::soc::IpcSocket sock_;
  static DtcMsgFactory factory_;
  uint16_t service_id;
};

}  // namespace dtc
}  // namespace diag
}  // namespace simba

#endif  // DIAG_DTC_CONTROLLER_DTC_CONTROLLER_H_
