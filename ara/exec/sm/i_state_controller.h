/**
 * @file i_state_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ARA_EXEC_SM_I_STATE_CONTROLLER_H_
#define ARA_EXEC_SM_I_STATE_CONTROLLER_H_

#include <cstdint>
#include <memory>

#include "ara/exec/sm/i_state.h"

namespace ara {
namespace exec {
namespace sm {
class IStateController {
 public:
  struct StateStatus {
    const uint16_t status_id_;
    const int8_t error_flag_;
  };

 public:
  /**
   * @brief Changing state to new with provided id
   *
   * @param new_state_id id of new state
   * @return int8_t 0 - OK
   */
  virtual int8_t ChangeState(const uint16_t& new_state_id) noexcept = 0;
  /**
   * @brief Adding state to DB
   *
   * @param state shared_ptr with new state
   * @return int8_t 0 - OK
   */
  virtual int8_t AddNewState(const std::shared_ptr<IState> state) noexcept = 0;
  /**
   * @brief Get the Current State object
   *
   * @return const uint16_t value of current state
   */
  virtual uint16_t GetCurrentState() const noexcept = 0;
  /**
   * @brief This method should be call to confirm changeing machine state
   *
   * @param event_value new state
   */
  virtual void StatusUpdateEvent(const StateStatus& event_value) noexcept = 0;
  /**
   * @brief This function will initialize Controller and set startup state
   * depend on config
   *
   */
  virtual void Init() noexcept = 0;
  /**
   * @brief This function should be call when we detect critical error
   *
   */
  virtual void Error() noexcept = 0;
  virtual ~IStateController() = default;
};
}  // namespace sm
}  // namespace exec
}  // namespace ara
#endif  // ARA_EXEC_SM_I_STATE_CONTROLLER_H_
