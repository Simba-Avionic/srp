/**
 * @file router.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "apps/example/router.h"

#include <memory>
#include <vector>

#include "communication-core/sockets/ipc_socket.h"
#include "core/logger/Logger.h"
#include "diag/base/controller/diag_controller.h"
namespace simba {
namespace router {

void Router::Run(const std::unordered_map<std::string, core::Parm>& parms) {
  diag::DiagController diag_controller{0x00001,
                                       std::make_unique<com::soc::IpcSocket>()};
  diag_controller.AddMethod(
      0x01,
      [this](const std::vector<uint8_t> payload) {
        std::string pp = "";
        for (auto a : payload) {
          pp += std::to_string(a) + ",";
        }
        AppLogger::Debug("Diag job 1 payload: " + pp);
        // return core::Result<std::vector<uint8_t>>{{0x01,0x02}};
        return core::Result<std::vector<uint8_t>>{};
      },
      diag::DiagMethodType::WRITE);
  diag_controller.Init();
  AppLogger::Debug("Router started");
  this->SleepMainThred();
  // this->logger_->Debug("Router started");
}
void Router::Stop() {}
}  // namespace router
}  // namespace simba
