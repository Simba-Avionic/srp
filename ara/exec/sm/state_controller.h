/**
 * @file state_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef ARA_EXEC_SM_STATE_CONTROLLER_H_
#define ARA_EXEC_SM_STATE_CONTROLLER_H_
#include <memory>
#include <unordered_map>
#include <atomic>

#include "ara/exec/sm/i_sm_com_wrapper.h"
#include "ara/exec/sm/i_state_controller.h"
namespace ara {
namespace exec {
namespace sm {
class StateController final : public IStateController {
 public:
  using StateIdType = std::uint16_t;
  using StateType = const std::shared_ptr<IState>;

 private:
  /**
   * @brief Hold id of current state
   *
   */
  std::atomic<StateIdType> current_state_;
  const StateIdType init_state_;
  /**
   * @brief Container with all state as map where key is state id
   *
   */
  std::unordered_map<StateIdType, StateType> state_container_{};

  /**
   * @brief Pointer on wrapper which provide communication with SM
   *
   */
  const std::unique_ptr<ISmComWrapper> com_wrapper_;

 public:
  explicit StateController(std::unique_ptr<ISmComWrapper> com_wrapper);
  StateController(std::unique_ptr<ISmComWrapper> com_wrapper,
                  const uint16_t& init_val);
  ~StateController() = default;

  explicit StateController(const StateController&) = delete;
  explicit StateController(const StateController&&) = delete;
  StateController operator=(const StateController&) = delete;
  StateController operator=(const StateController&&) = delete;

  /**
   * @brief Changing state to new with provided id
   *
   * @param new_state_id id of new state
   * @return int8_t 0 - OK
   */
  int8_t ChangeState(const uint16_t& new_state_id) noexcept override;

  /**
   * @brief Adding state to DB
   *
   * @param state unique_ptr with new state
   * @return int8_t 0 - OK
   */
  int8_t AddNewState(const std::shared_ptr<IState> state) noexcept override;

  /**
   * @brief Get the Current State object
   *
   * @return uint16_t value of current state
   */
  uint16_t GetCurrentState() const noexcept override;
  /**
   * @brief This method should be call to confirm changeing machine state
   *
   * @param event_value new state
   */
  void StatusUpdateEvent(
      const IStateController::StateStatus& event_value) noexcept override;

  /**
   * @brief This function will initialize Controller and set startup state
   * depend on config
   *
   */
  void Init() noexcept override;
  /**
   * @brief This function should be call when we detect critical error
   *
   */
  void Error() noexcept override;
};
}  // namespace sm
}  // namespace exec
}  // namespace ara

#endif  // ARA_EXEC_SM_STATE_CONTROLLER_H_
