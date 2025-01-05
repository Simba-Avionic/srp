/**
 * @file state.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_EXEC_SM_STATE_H_
#define ARA_EXEC_SM_STATE_H_

#include <cstdint>
#include <utility>

#include "ara/exec/sm/i_state.h"

namespace ara {
namespace exec {
namespace sm {

class State : public IState {
 protected:
  const uint16_t state_id_;
  IState::ChangeStatusCallback change_status_callback_;
  IState::DtcCallback dtc_callback_;
  void DtcSetStatus(const bool status) noexcept { dtc_callback_(status); }
  void SwitchState(const uint16_t new_state) noexcept {
    this->change_status_callback_(new_state);
  }

 public:
  State(const uint16_t id, const bool reset_flag, const bool startup_flag) : state_id_{id} {}
  uint16_t GetStateId() const noexcept override { return state_id_; }
  void SetCallback(IState::ChangeStatusCallback&& s_callback,
                   IState::DtcCallback&& d_callback) noexcept override {
    this->change_status_callback_ = std::move(s_callback);
    this->dtc_callback_ = std::move(d_callback);
  }
  virtual ~State() = default;
};

}  // namespace sm
}  // namespace exec
}  // namespace ara

#endif  // ARA_EXEC_SM_STATE_H_
