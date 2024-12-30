/**
 * @file state_manager.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "platform/common/state_manager/code/state_manager.h"

#include <string_view>

#include "ara/log/log.h"
#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "simba/platform/sm/StateManager/StateManagerHandler.h"
namespace simba {
namespace sm {
namespace {
static constexpr std::string_view kConfigPath{
    "/srp/opt/cpu_simba/machine_states.json"};
}  // namespace

StateManager::StateManager(/* args */) {}

StateManager::~StateManager() {}

int StateManager::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  const auto& file =
      simba::core::json::JsonParser::Parser(std::string{kConfigPath});
  if (!file.has_value()) {
    ara::log::LogError() << "Missing config file!";
    return -1;
  }
  const auto& json = file.value();
  const auto machine_state = json.GetArray<std::string>("machine_state");
  if (!machine_state.has_value()) {
    ara::log::LogError() << "Incorrect json structure!";
    return -2;
  }
  ara::log::LogInfo() << "App Initialized";
  return 0;
}

int StateManager::Run(const std::stop_token& token) {
  simba::platform::sm::StateManagerProxy sm_proxy{ara::core::InstanceSpecifier{
      "simba/platform/state_manager/SmServiceRPort"}};
  sm_proxy.StartFindService([](auto handler) {
    ara::log::LogInfo() << "Service Founded";
    handler->SetState(35698U);
    handler->SetState(11884U);
    handler->SetState(43499U);
    handler->SetState(46617U);
  });
  ara::log::LogInfo() << "App Started";
  simba::core::condition::wait(token);
  ara::log::LogInfo() << "App Stoping";
  return 0;
}

}  // namespace sm
}  // namespace simba
