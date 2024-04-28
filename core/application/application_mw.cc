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
  this->DiagConfig(parms);
  AppLogger::Info("Application [MW] configured");
  AppLogger::Info("Application [MW] initialize");
  this->Initialize(parms);

  AppLogger::Info("Application [MW] running");
  this->Run(this->source.get_token());
}
ErrorCode ApplicationMW::MwConfig(
    const std::unordered_map<std::string, std::string>& parms) {
  auto obj = json::JsonParser::Parser("/opt/" + parms.at("app_name") +
                                      "/etc/srp_app.json")
                 .value();
  auto app_ = obj.GetNumber<uint16_t>("app_id");
  if (app_.has_value()) {
    this->app_id_ = app_.value();
    this->exec_.Init(app_.value());
  } else {
    return ErrorCode::kError;
  }
  return ErrorCode::kOk;
}
}  // namespace core
}  // namespace simba
