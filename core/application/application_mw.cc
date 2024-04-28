/**
 * @file application_mw.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "core/application/application_mw.h"

#include "communication-core/sockets/ipc_socket.h"
#include "core/json/json_parser.h"
#include "core/logger/Logger.h"
#include "diag/base/controller/diag_controller.h"

namespace simba {
namespace core {
void ApplicationMW::onRun(
    const std::unordered_map<std::string, std::string>& parms) {
  if (this->CommonConfig(parms) != ErrorCode::kOk) {
    std::abort();
  }
  AppLogger::Info("Application [MW]");
  if (this->MwConfig(parms) != ErrorCode::kOk) {
    std::abort();
  }
  this->SomeIPConfig(parms);
  AppLogger::Info("Application [MW] configured");
  AppLogger::Info("Application [MW] initialize");
  this->Initialize(parms);
  if (this->diag_controller != nullptr) {
    this->diag_controller->Init();
    AppLogger::Info("Application [MW] diag initialized");
  }
  AppLogger::Info("Application [MW] running");
  this->Run(this->source.get_token());
}
ErrorCode ApplicationMW::MwConfig(
    const std::unordered_map<std::string, std::string>& parms) {
  auto obj = json::JsonParser::Parser("/opt/" + parms.at("app_name") +
                                      "/etc/srp_app.json")
                 .value();
  auto service_id_r = obj.GetNumber<uint16_t>("diag_id");
  if (service_id_r.has_value()) {
    if (service_id_r.value() != 0) {
      AppLogger::Info("Application [MW] Service_id: " +
                      std::to_string(service_id_r.value()));
      this->diag_controller = std::make_unique<diag::DiagController>(
          service_id_r.value(), std::make_unique<com::soc::IpcSocket>());
    }
  }
  auto app_ = obj.GetNumber<uint16_t>("app_id");
  if (app_.has_value()) {
    this->app_id_ = app_.value();
    this->exec_.Init(app_.value());
    this->app_id_ = app_.value();
  } else {
    return ErrorCode::kError;
  }
  return ErrorCode::kOk;
}
}  // namespace core
}  // namespace simba
