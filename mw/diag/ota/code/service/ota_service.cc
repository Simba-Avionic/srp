/**
 * @file ota_service.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-05
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "mw/diag/ota/code/service/ota_service.h"

#include "communication-core/sockets/ipc_socket.h"
#include "core/results/result.h"
#include "diag/base/controller/diag_controller.h"
namespace simba {
namespace mw {
namespace ota {
void OtaService::Run(const std::unordered_map<std::string, core::Parm>& parms) {
  this->diag_controller = std::make_unique<diag::DiagController>(
      0x0100, std::make_unique<com::soc::IpcSocket>());
  diag_controller->Init();
  diag_controller->AddMethod(
      0x01,
      [this](const std::vector<uint8_t> payload) {
        return core::Result<std::vector<uint8_t>>{};
      },
      diag::DiagMethodType::WRITE);
  AppLogger::Info("Application function started");
  std::exit(0);
}
OtaService::OtaService() { this->app_name = "OTA_MW"; }
}  // namespace ota
}  // namespace mw
}  // namespace simba
