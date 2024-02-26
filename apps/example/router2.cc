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
#include <memory>

#include "apps/example/router.h"
#include "communication-core/sockets/ipc_socket.h"
#include "core/logger/Logger.h"
#include "diag/base/controller/diag_controller.h"
namespace simba {
namespace router {

void Router::Run(const std::unordered_map<std::string, core::Parm>& parms) {
  diag::DiagController diag_controller{0x00002,
                                       std::make_unique<com::soc::IpcSocket>()};
  diag_controller.Init();
  const auto res = diag_controller.Write(0x0001, 0x01, {0x10, 0x01});
  if (res == core::ErrorCode::kOk) {
    AppLogger::Info("OK");
  } else {
    AppLogger::Error("NOK");
  }
  //   if(res.HasValue()){
  //     std::string pp = "";
  //     auto res_p = res.Value();
  //     for(auto a : res_p){
  //         pp+=std::to_string(a)+",";
  //     }
  //     AppLogger::Debug("Payload: "+pp);
  //   }

  AppLogger::Debug("Router started");
  this->SleepMainThread();
  // this->logger_->Debug("Router started");
}
void Router::Stop() {}
}  // namespace router
}  // namespace simba
