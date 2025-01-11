/**
 * @file state_manager.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PLATFORM_COMMON_STATE_MANAGER_CODE_STATE_MANAGER_H_
#define PLATFORM_COMMON_STATE_MANAGER_CODE_STATE_MANAGER_H_

#include <condition_variable>  // NOLINT
#include <map>
#include <memory>
#include <queue>
#include <string>

#include "ara/exec/adaptive_application.h"
#include "ara/exec/sm/i_state_controller.h"
#include "platform/common/state_manager/code/did_impl.h"

namespace srp {
namespace sm {
class StateManager final : public ara::exec::AdaptiveApplication {
 private:
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                     parms) override;
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  std::shared_ptr<ara::exec::sm::IStateController> state_con_;
  std::unique_ptr<DiDImpl> did_;
 public:
  StateManager(/* args */);
  ~StateManager();
};
}  // namespace sm
}  // namespace srp
#endif  // PLATFORM_COMMON_STATE_MANAGER_CODE_STATE_MANAGER_H_
