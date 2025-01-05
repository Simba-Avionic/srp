/**
 * @file test_generated_state_controller.cc
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

#include "ara/exec/sm/initialization.h"
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
  const std::shared_ptr<IStateController> sut_;

 public:
  StateControllerFixture(/* args */)
      : mock_{std::make_shared<SmComWrapperMock>()}, sut_{} {}
};

TEST_F(StateControllerFixture, InitEmpty) {
  auto sut_ = Initialize(std::make_unique<SmComWrapperFacade>(mock_));
  EXPECT_CALL(*mock_, SendNewState(35698U)).WillOnce(::testing::Return(0));
  EXPECT_EQ(0x00, sut_->GetCurrentState());
  sut_->Init();
  EXPECT_EQ(35698U, sut_->GetCurrentState());
}

TEST_F(StateControllerFixture, Positive) {
  auto sut_ = Initialize(std::make_unique<SmComWrapperFacade>(mock_));
  EXPECT_CALL(*mock_, SendNewState(35698U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SendNewState(11884U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SendNewState(43499U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SendNewState(46617U)).WillOnce(::testing::Return(0));
  EXPECT_EQ(0x00, sut_->GetCurrentState());
  sut_->Init();
  EXPECT_EQ(35698U, sut_->GetCurrentState());
  sut_->StatusUpdateEvent(IStateController::StateStatus{35698U, 0x00});
  EXPECT_EQ(11884U, sut_->GetCurrentState());
  sut_->StatusUpdateEvent(IStateController::StateStatus{11884U, 0x00});
  EXPECT_EQ(43499U, sut_->GetCurrentState());
  sut_->StatusUpdateEvent(IStateController::StateStatus{43499U, 0x00});
  EXPECT_EQ(46617U, sut_->GetCurrentState());
  sut_->StatusUpdateEvent(IStateController::StateStatus{46617U, 0x00});
  EXPECT_EQ(46617U, sut_->GetCurrentState());
}

TEST_F(StateControllerFixture, PositiveAndErrorAtTheEnd) {
  auto sut_ = Initialize(std::make_unique<SmComWrapperFacade>(mock_));
  EXPECT_CALL(*mock_, SendNewState(35698U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SendNewState(11884U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SendNewState(43499U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SendNewState(46617U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SendNewState(46771U)).WillOnce(::testing::Return(0));
  EXPECT_CALL(*mock_, SetDtcStatus(true)).Times(1);
  EXPECT_EQ(0x00, sut_->GetCurrentState());
  sut_->Init();
  EXPECT_EQ(35698U, sut_->GetCurrentState());
  sut_->StatusUpdateEvent(IStateController::StateStatus{35698U, 0x00});
  EXPECT_EQ(11884U, sut_->GetCurrentState());
  sut_->StatusUpdateEvent(IStateController::StateStatus{11884U, 0x00});
  EXPECT_EQ(43499U, sut_->GetCurrentState());
  sut_->StatusUpdateEvent(IStateController::StateStatus{43499U, 0x00});
  EXPECT_EQ(46617U, sut_->GetCurrentState());
  sut_->StatusUpdateEvent(IStateController::StateStatus{46617U, 0x00});
  EXPECT_EQ(46617U, sut_->GetCurrentState());
  sut_->Error();
  EXPECT_EQ(46771U, sut_->GetCurrentState());
}

}  // namespace sm
}  // namespace exec
}  // namespace ara
