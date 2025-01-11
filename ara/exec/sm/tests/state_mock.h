/**
 * @file state_mock.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ARA_EXEC_SM_TESTS_STATE_MOCK_H_
#define ARA_EXEC_SM_TESTS_STATE_MOCK_H_
#include <gmock/gmock.h>

#include <memory>
#include <utility>

#include "ara/exec/sm/state.h"
namespace ara {
namespace exec {
namespace sm {
class StateMock : public State {
 private:
  /* data */
 public:
  StateMock(uint16_t id, bool reset_flag, bool startup_flag)
      : State{id, reset_flag, startup_flag} {}
  MOCK_METHOD(void, Finish, (), (noexcept, override));
  MOCK_METHOD(void, Error, (), (noexcept, override));
  MOCK_METHOD(void, DiDHandler, (const uint8_t), (noexcept, override));
};
class StateFacade : public IState {
 private:
  const std::shared_ptr<StateMock> mock_;

 public:
  void Finish() noexcept override { mock_->Finish(); }
  void Error() noexcept override { mock_->Error(); }
  void DiDHandler(const uint8_t val) noexcept override {
    mock_->DiDHandler(val);
  }
  uint16_t GetStateId() const noexcept override { return mock_->GetStateId(); }
  void SetCallback(ChangeStatusCallback&& s_callback,
                   DtcCallback&& d_callback) noexcept override {
    this->mock_->SetCallback(std::move(s_callback), std::move(d_callback));
  }
  explicit StateFacade(std::shared_ptr<StateMock> mock) : mock_{mock} {};
};
}  // namespace sm
}  // namespace exec
}  // namespace ara

#endif  // ARA_EXEC_SM_TESTS_STATE_MOCK_H_
