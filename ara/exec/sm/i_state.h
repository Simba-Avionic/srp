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
#include <functional>

namespace ara {
namespace exec {
namespace sm {

class IState {
 public:
  using ChangeStatusCallback = std::function<void(const uint16_t&)>;
  using DtcCallback = std::function<void(const bool&)>;

 public:
  virtual void Finish() noexcept = 0;
  virtual void Error() noexcept = 0;
  virtual void DiDHandler(const uint8_t val) noexcept = 0;
  virtual uint16_t GetStateId() const noexcept = 0;
  virtual void SetCallback(ChangeStatusCallback&& s_callback,
                           DtcCallback&& d_callback) noexcept = 0;
  virtual ~IState() = default;
};

}  // namespace sm
}  // namespace exec
}  // namespace ara
#endif  // ARA_EXEC_SM_I_STATE_H_
