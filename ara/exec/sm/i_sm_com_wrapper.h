/**
 * @file i_sm_com_wrapper.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ARA_EXEC_SM_I_SM_COM_WRAPPER_H_
#define ARA_EXEC_SM_I_SM_COM_WRAPPER_H_

#include <cstdint>

namespace ara {
namespace exec {
namespace sm {
class ISmComWrapper {
 public:
  /**
   * @brief Sending/providing new status to destination
   *
   * @param new_state new state id
   * @return int8_t 0-OK
   */
  virtual int8_t SendNewState(const uint16_t& new_state) noexcept = 0;
  /**
   * @brief Set the Dtc Status
   *
   * @param new_status new status
   * @return int8_t  0-OK
   */
  virtual int8_t SetDtcStatus(const bool& new_status) noexcept = 0;
  virtual ~ISmComWrapper() = default;
};
}  // namespace sm
}  // namespace exec
}  // namespace ara
#endif  // ARA_EXEC_SM_I_SM_COM_WRAPPER_H_
