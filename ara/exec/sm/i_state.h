/**
 * @file i_state.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_EXEC_SM_I_STATE_H_
#define ARA_EXEC_SM_I_STATE_H_

#include <cstdint>

namespace ara {
namespace exec {
namespace sm {

class IState {
 public:
  virtual void Finish() noexcept = 0;
  virtual void Error() noexcept = 0;
  virtual void DiDHandler(const uint8_t val) noexcept = 0;

  virtual ~IState() = default;
};

}  // namespace sm
}  // namespace exec
}  // namespace ara
#endif  // ARA_EXEC_SM_I_STATE_H_
