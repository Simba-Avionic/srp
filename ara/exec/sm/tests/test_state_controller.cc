/**
 * @file test_state_controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <gtest/gtest.h>

#include <memory>

#include "ara/exec/sm/state_controller.h"
#include "ara/exec/sm/tests/com_wrapper_mock.h"
#include "ara/exec/sm/tests/state_mock.h"

namespace ara {
namespace exec {
namespace sm {

class StateControllerFixture : public ::testing::Test {
 protected:
  const std::shared_ptr<SmComWrapperMock> mock_;
  std::unique_ptr<SmComWrapperFacade> CreateWrapper() {
    return std::make_unique<SmComWrapperFacade>(mock_);
  }

 public:
  StateControllerFixture(/* args */)
      : mock_{std::make_shared<SmComWrapperMock>()} {}
};

TEST_F(StateControllerFixture, InitEmpty) {
  StateController sut_{std::move(this->CreateWrapper())};

  EXPECT_CALL(*mock_, SendNewState(::testing::_)).Times(0);
  EXPECT_CALL(*mock_, SetDtcStatus(::testing::_)).Times(0);

  EXPECT_EQ(-2, sut_.ChangeState(11120));
  EXPECT_EQ(0x0, sut_.GetCurrentState());
}

TEST_F(StateControllerFixture, CheckWithOneState) {
  auto state_mock_ = std::make_shared<StateMock>(0x01, false, false);
  StateController sut_{std::move(this->CreateWrapper())};

  EXPECT_CALL(*mock_, SendNewState(0x01U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SetDtcStatus(::testing::_)).Times(0);

  EXPECT_EQ(0x0, sut_.AddNewState(state_mock_));
  EXPECT_EQ(0x0, sut_.ChangeState(0x01U));
  EXPECT_EQ(0x01, sut_.GetCurrentState());
  EXPECT_EQ(-2, sut_.ChangeState(0x02U));
}

TEST_F(StateControllerFixture, CheckUpdateWithoutError) {
  auto state_mock_ = std::make_shared<StateMock>(0x01, false, false);
  StateController sut_{std::move(this->CreateWrapper())};

  EXPECT_CALL(*mock_, SendNewState(0x01U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SetDtcStatus(::testing::_)).Times(0);

  EXPECT_CALL(*state_mock_, Finish()).Times(1);
  EXPECT_CALL(*state_mock_, Error()).Times(0);
  EXPECT_CALL(*state_mock_, DiDHandler(::testing::_)).Times(0);

  EXPECT_EQ(0x0, sut_.AddNewState(state_mock_));
  EXPECT_EQ(0x0, sut_.ChangeState(0x01U));
  EXPECT_EQ(0x01, sut_.GetCurrentState());
  sut_.StatusUpdateEvent(IStateController::StateStatus{0x01, 0x0});
}

TEST_F(StateControllerFixture, CheckUpdateWithError) {
  auto state_mock_ = std::make_shared<StateMock>(0x01, false, false);
  StateController sut_{std::move(this->CreateWrapper())};

  EXPECT_CALL(*mock_, SendNewState(0x01U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SetDtcStatus(::testing::_)).Times(0);

  EXPECT_CALL(*state_mock_, Finish()).Times(0);
  EXPECT_CALL(*state_mock_, Error()).Times(1);
  EXPECT_CALL(*state_mock_, DiDHandler(::testing::_)).Times(0);

  EXPECT_EQ(0x0, sut_.AddNewState(state_mock_));
  EXPECT_EQ(0x0, sut_.ChangeState(0x01U));
  EXPECT_EQ(0x01, sut_.GetCurrentState());
  sut_.StatusUpdateEvent(IStateController::StateStatus{0x01, 0x1});
}
}  // namespace sm
}  // namespace exec
}  // namespace ara
