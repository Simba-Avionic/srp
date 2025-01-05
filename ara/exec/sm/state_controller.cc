/**
 * @file state_controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "ara/exec/sm/state_controller.h"

#include <memory>
#include <utility>

namespace ara {
namespace exec {
namespace sm {
namespace {
constexpr uint16_t kInitState{0x0U};
}  // namespace

StateController::StateController(std::unique_ptr<ISmComWrapper> com_wrapper)
    : current_state_{kInitState},
      init_state_{kInitState},
      state_container_{},
      com_wrapper_{std::move(com_wrapper)} {}

StateController::StateController(std::unique_ptr<ISmComWrapper> com_wrapper,
                                 const uint16_t& init_val)
    : current_state_{kInitState},
      init_state_{init_val},
      state_container_{},
      com_wrapper_{std::move(com_wrapper)} {}

int8_t StateController::ChangeState(const uint16_t& new_state_id) noexcept {
  if (this->current_state_ == new_state_id) {
    return -1;
  }
  const auto& iter = this->state_container_.find(new_state_id);
  if (iter == this->state_container_.end()) {
    return -2;
  }
  this->current_state_ = new_state_id;

  if (this->com_wrapper_) {
    std::ignore = this->com_wrapper_->SendNewState(this->current_state_);
  }
  return 0;
}

int8_t StateController::AddNewState(
    const std::shared_ptr<IState> state) noexcept {
  const auto res =
      this->state_container_.insert({state->GetStateId(), state}).second;
  state->SetCallback(
      [this](const auto& new_id) { std::ignore = this->ChangeState(new_id); },
      [this](const auto& new_status) {
        if (this->com_wrapper_) {
          this->com_wrapper_->SetDtcStatus(new_status);
        }
      });
  return res ? 0 : -1;
}

uint16_t StateController::GetCurrentState() const noexcept {
  return this->current_state_;
}

void StateController::StatusUpdateEvent(
    const IStateController::StateStatus& event_value) noexcept {
  const auto& iter = this->state_container_.find(this->current_state_);
  if (iter == this->state_container_.end()) {
    return;
  }
  if (event_value.error_flag_ == 0x0) {
    iter->second->Finish();
  } else {
    iter->second->Error();
  }
}
void StateController::Init() noexcept {
  if (init_state_ != 0x00U && current_state_ == 0x00U) {
    this->ChangeState(init_state_);
  }
}

void StateController::Error() noexcept {
  const auto& iter = this->state_container_.find(this->current_state_);
  if (iter == this->state_container_.end()) {
    return;
  }
  iter->second->Error();
}

}  // namespace sm
}  // namespace exec
}  // namespace ara
