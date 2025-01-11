/**
 * @file em_application.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/em/code/em_application.h"

#include <map>
#include <string>

#include "ara/core/instance_specifier.h"
#include "ara/log/log.h"
#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "platform/common/em/code/services/em/app_db.h"

namespace srp {
namespace em {
namespace {
static const ara::core::InstanceSpecifier kSmServiceInstance{
    "srp/platform/em/SmServicePPort"};
}  // namespace

EmApplication::EmApplication(/* args */)
    : sm_service_{kSmServiceInstance, [this](uint16_t new_state) {
                    this->cmd_list_.push(new_state);
                    return 0;
                  }} {}

EmApplication::~EmApplication() {}

int EmApplication::Run(const std::stop_token& token) {
  // this->em_service->StartApps();
  sm_service_.StartOffer();
  while (!token.stop_requested()) {
    const auto val = this->cmd_list_.Get(token);
    if (val.has_value()) {
      this->em_service->SetActiveState(val.value());
    }
  }
  sm_service_.StopOffer();
  return 0;
}
/**
 * @brief This function is called to initialize the application
 *
 * @param parms map with parms
 */
int EmApplication::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  const auto db = std::make_shared<service::data::AppDb>();
  em_service =
      std::make_shared<service::EmService>(db, [this](const uint16_t& new_id) {
        this->sm_service_.CurrentState.Update(new_id);
      });

  const auto json_opt = srp::core::json::JsonParser::Parser(
      std::string{"/srp/platform/etc/state_config.json"});
  if (!json_opt.has_value()) {
    return -1;
  }
  const srp::core::json::JsonParser json = json_opt.value();

  const auto states_opt = json.GetArray<nlohmann::json>("states");
  if (!states_opt.has_value()) {
    ara::log::LogError() << "Incorrect states type";
    return -1;
  }
  const auto states = states_opt.value();
  for (const auto& item : states) {
    srp::core::json::JsonParser state =
        srp::core::json::JsonParser::Parser(item).value();
    std::string name = state.GetString("name").value();
    uint16_t id = state.GetNumber<uint16_t>("id").value();
    ara::log::LogInfo() << "New state: " << name << "[" << id << "]";
    db->InsertNewFG(id, name);
  }
  this->em_service->LoadApps();
  return 0;
}

}  // namespace em
}  // namespace srp
