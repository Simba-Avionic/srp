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

#include "ara/exec/sm/i_state.h"

namespace ara {
namespace exec {
namespace sm {

class State : public IState {
 protected:
  void DtcSetStatus(bool status) noexcept {}
  void SwitchState(const uint16_t new_state) noexcept {}

 public:
  State(bool reset_flag, bool startup_flag) {}
  virtual ~State() = default;
};

}  // namespace sm
}  // namespace exec
}  // namespace ara

#endif  // ARA_EXEC_SM_STATE_H_
