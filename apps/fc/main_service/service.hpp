/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_FC_MAIN_SERVICE_SERVICE_HPP_
#define APPS_FC_MAIN_SERVICE_SERVICE_HPP_
#include <memory>
#include <unordered_map>
#include <vector>
#include "srp/apps/MainServiceSkeleton.h"
#include "apps/fc/main_service/rocket_state.h"
#include "ara/log/log.h"
namespace srp {
namespace apps {
// Transitions allowed from external like GS
static const std::unordered_map<RocketState_t, std::vector<RocketState_t>> allowed_transitions{
    {RocketState_t::DISARM,       {RocketState_t::ARM, RocketState_t::ABORT}},
    {RocketState_t::ARM,          {RocketState_t::LAUNCH, RocketState_t::ABORT, RocketState_t::DISARM}},
    {RocketState_t::ABORT,        {RocketState_t::DISARM}}
};
class MyMainServiceSkeleton: public MainServiceSkeleton {
 private:
  std::shared_ptr<RocketState> state;
  inline void CaptureError(const RocketState_t req_state, const RocketState_t actual_state) const {
    if (req_state == RocketState_t::DISARM && actual_state == RocketState_t::INIT) {
      ara::log::LogError() <<
          "DISARM from INIT: This change needs to be called automatically by mainService, not from someip!";
    }
  }

 public:
  explicit MyMainServiceSkeleton(const ara::core::InstanceSpecifier& instance):
                MainServiceSkeleton{instance} {
    state = RocketState::GetInstance();
  }
  virtual ~MyMainServiceSkeleton() = default;

 protected:
  ara::core::Result<bool> setMode(const std::uint8_t& in_parm) override {
    const auto req_state = static_cast<RocketState_t>(in_parm);
    const auto actual_state = state->GetState();
    this->CaptureError(req_state, actual_state);
    const auto it = allowed_transitions.find(actual_state);
    if (it != allowed_transitions.end() &&
        std::find(it->second.begin(), it->second.end(), req_state) != it->second.end()) {
        state->SetState(req_state);
        CurrentModeStatusEvent.Update(static_cast<uint8_t>(req_state));
        return true;
    }
    ara::log::LogError() << "Incorrect transactions from: " << RocketState::GetStringState(actual_state)
                          << ", to state: " << RocketState::GetStringState(req_state);
    return false;
  }
};
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_MAIN_SERVICE_SERVICE_HPP_
