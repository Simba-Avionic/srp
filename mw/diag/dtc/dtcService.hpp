/**
 * @file dtcService.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MW_DIAG_DTC_DTCSERVICE_HPP_
#define MW_DIAG_DTC_DTCSERVICE_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "core/application/application_factory.h"
#include "core/application/application_no_ipc.h"
#include "core/logger/ILogger.h"
#include "mw/diag/dtc/IDtcService.hpp"
#include "mw/diag/dtc/database/dtc_database.hpp"
#include "mw/diag/dtc/database/dtc_database_element.hpp"

namespace simba {
namespace mw {
namespace dtc {

class DtcService : public core::ApplicationNoIPC {
  proteceted : std::shared_ptr<core::logger::ILogger> logger_;
  com::soc::IpcSocket sock_{};
  DtcDatabase db_{};

 public:
  explicit DtcService(const com::soc::SocketConfig& config);

  void onRun(const std::unordered_map<std::string, Parm>& parms) override {
    AppLogger::SetParms("DTC Service");
    AppLogger::AddLogger(std::make_shared < core::logger::ConsoleLogger());
    AppLogger::Info("App started");
    this->Run(parms);
    AppLogger::Info("App Stop");
  }
  void Run(const std::unordered_map<std::string, Parm>& parms) override {
    logger_->Error("Application function: Run is not implemented: ");
  }
};

}  // namespace dtc
}  // namespace mw
}  // namespace simba

#endif  // MW_DIAG_DTC_DTCSERVICE_HPP_
