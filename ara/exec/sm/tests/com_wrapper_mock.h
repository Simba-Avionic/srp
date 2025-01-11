/**
 * @file com_wrapper_mock.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ARA_EXEC_SM_TESTS_COM_WRAPPER_MOCK_H_
#define ARA_EXEC_SM_TESTS_COM_WRAPPER_MOCK_H_
#include <gmock/gmock.h>

#include <memory>

#include "ara/exec/sm/i_sm_com_wrapper.h"
namespace ara {
namespace exec {
namespace sm {
class SmComWrapperMock : public ISmComWrapper {
 public:
  MOCK_METHOD(int8_t, SendNewState, (const uint16_t& new_state),
              (noexcept, override));
  MOCK_METHOD(int8_t, SetDtcStatus, (const bool& new_status),
              (noexcept, override));
};
class SmComWrapperFacade : public ISmComWrapper {
 private:
  const std::shared_ptr<SmComWrapperMock> mock_;

 public:
  explicit SmComWrapperFacade(std::shared_ptr<SmComWrapperMock> mock)
      : mock_{mock} {}
  int8_t SendNewState(const uint16_t& new_state) noexcept override {
    return mock_->SendNewState(new_state);
  }
  int8_t SetDtcStatus(const bool& new_status) noexcept override {
    return mock_->SetDtcStatus(new_status);
  }
};
}  // namespace sm
}  // namespace exec
}  // namespace ara

#endif  // ARA_EXEC_SM_TESTS_COM_WRAPPER_MOCK_H_
