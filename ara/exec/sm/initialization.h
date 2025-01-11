/**
 * @file initialization.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ARA_EXEC_SM_INITIALIZATION_H_
#define ARA_EXEC_SM_INITIALIZATION_H_
#include <memory>

#include "ara/exec/sm/i_sm_com_wrapper.h"
#include "ara/exec/sm/i_state_controller.h"
namespace ara {
namespace exec {
namespace sm {
std::shared_ptr<IStateController> Initialize(
    std::unique_ptr<ISmComWrapper> com_wrapper) noexcept;
}  // namespace sm
}  // namespace exec
}  // namespace ara
#endif  // ARA_EXEC_SM_INITIALIZATION_H_
