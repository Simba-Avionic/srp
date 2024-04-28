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
#include <utility>
#include <vector>

#include "communication-core/sockets/ipc_socket.h"
#include "diag/base/controller/diag_controller.h"
#include "diag/base/controller/idiag_controller.h"
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
  void Init(uint16_t service_id) {
    dtc_controller.Init(service_id);
    diag_controller = std::make_unique<diag::DiagController>(
        service_id, std::make_unique<com::soc::IpcSocket>());
    diag_controller->Init();
  }
  simba::core::ErrorCode AddMethod(const uint8_t diag_id, DiagMethod callback,
                                   const DiagMethodType method_type) {
    return diag_controller->AddMethod(std::move(diag_id), std::move(callback),
                                      std::move(method_type));
  }
  std::optional<std::vector<uint8_t>> Read(const uint16_t service_id,
                                           const uint8_t diag_id) {
    return diag_controller->Read(std::move(service_id), std::move(diag_id));
  }
  simba::core::ErrorCode Write(const uint16_t service_id, const uint8_t diag_id,
                               const std::vector<uint8_t> payload) {
    return diag_controller->Write(std::move(service_id), std::move(diag_id),
                                  std::move(payload));
  }
  std::optional<std::vector<uint8_t>> Job(const uint16_t service_id,
                                          const uint8_t diag_id,
                                          const std::vector<uint8_t> payload) {
    return diag_controller->Job(std::move(service_id), std::move(diag_id),
                                std::move(payload));
  }

 private:
  dtc::DtcController dtc_controller{};
  std::unique_ptr<diag::DiagController> diag_controller;
};
}  // namespace diag
}  // namespace simba

#endif  // DIAG_CONTROLLER_DIAG_CONTROLLER_H_
