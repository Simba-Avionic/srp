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

#include "ara/exec/sm/initialization.h"
#include "ara/log/log.h"
#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "platform/common/state_manager/code/com_wrapper.h"
#include "srp/platform/sm/StateManager/StateManagerHandler.h"

namespace srp {
namespace sm {
namespace {
static constexpr std::string_view kConfigPath{
    "/srp/opt/cpu_srp/machine_states.json"};
static const ara::core::InstanceSpecifier did_instance_{
    "/srp/platform/state_manager/CurrentMachineStatePPort"};
}  // namespace

StateManager::StateManager(/* args */) {}

StateManager::~StateManager() {}

int StateManager::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  const auto& file =
      srp::core::json::JsonParser::Parser(std::string{kConfigPath});
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
  srp::platform::sm::StateManagerProxy sm_proxy{ara::core::InstanceSpecifier{
      "srp/platform/state_manager/SmServiceRPort"}};
  sm_proxy.StartFindService(
      [this](srp::platform::sm::StateManagerProxy::handler_t handler) {
        ara::log::LogDebug() << "Service Founded";

        handler->CurrentState.Subscribe(1, [this, handler](const auto) {
          handler->CurrentState.SetReceiveHandler([this, handler]() {
            const auto val = handler->CurrentState.GetNewSamples();
            ara::log::LogInfo() << " SM d1";
            if (val.HasValue()) {
              ara::log::LogInfo() << "Event Value: " << val.Value();
              this->state_con_->StatusUpdateEvent({val.Value(), 0});
            }
          });
        });

        this->state_con_ =
            ara::exec::sm::Initialize(std::make_unique<ComWrapper>(handler));
        this->state_con_->Init();
        this->did_ = std::make_unique<DiDImpl>(did_instance_, this->state_con_);
        this->did_->Offer();
      });
  ara::log::LogInfo() << "App Started";
  srp::core::condition::wait(token);
  sm_proxy.StopFindService();
  if (did_) {
    did_->StopOffer();
  }
  ara::log::LogInfo() << "App Stoping";
  return 0;
}

}  // namespace sm
}  // namespace srp
