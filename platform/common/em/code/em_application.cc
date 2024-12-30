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
#include "platform/common/em/code/services/sm_service.h"

namespace simba {
namespace em {
namespace {
static const ara::core::InstanceSpecifier kSmServiceInstance{
    "simba/platform/em/SmServicePPort"};
}  // namespace

EmApplication::EmApplication(/* args */) {}

EmApplication::~EmApplication() {}

int EmApplication::Run(const std::stop_token& token) {
  // this->em_service->StartApps();
  service::SmService sm_service{kSmServiceInstance, [this](uint16_t new_state) {
                                  em_service->SetActiveState(new_state);
                                  return 0;
                                }};
  sm_service.StartOffer();
  core::condition::wait(token);
  sm_service.StopOffer();
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
  em_service = std::make_shared<service::EmService>(db);

  const auto json_opt = simba::core::json::JsonParser::Parser(
      std::string{"/srp/platform/etc/state_config.json"});
  if (!json_opt.has_value()) {
    return -1;
  }
  const simba::core::json::JsonParser json = json_opt.value();

  const auto states_opt = json.GetArray<nlohmann::json>("states");
  if (!states_opt.has_value()) {
    ara::log::LogError() << "Incorrect states type";
    return -1;
  }
  const auto states = states_opt.value();
  for (const auto& item : states) {
    simba::core::json::JsonParser state =
        simba::core::json::JsonParser::Parser(item).value();
    std::string name = state.GetString("name").value();
    uint16_t id = state.GetNumber<uint16_t>("id").value();
    ara::log::LogInfo() << "New state: " << name << "[" << id << "]";
    db->InsertNewFG(id, name);
  }
  this->em_service->LoadApps();
  return 0;
}

}  // namespace em
}  // namespace simba
