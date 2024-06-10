/**
 * @file test_temp_controller.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Unit tests 
 * @version 0.1
 * @date 2024-06-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <memory>
#include <gtest/gtest.h>

#include "mw/temp/controller/temp_controller.h"
#include "communication-core/sockets/mock/mockSocket.h"

class TestWrapper : public simba::mw::temp::TempController {
 public:
  simba::core::ErrorCode TestInit(uint16_t service_id, std::unique_ptr<MockSocket> socket) {
      return simba::core::ErrorCode::kOk;
  }
  simba::core::ErrorCode TestSubscribe() {
    return this->Subscribe();
  }
};

class TestTempSub : public ::testing::TestWithParam<
    uint16_t>{
};

INSTANTIATE_TEST_SUITE_P(TestTempSubParam, TestTempSub,
::testing::Values(
    0
));

TEST_P(TestTempSub, TEMP_SUBSCRIBE_TEST) {
    TestWrapper wrapper{};
    // uint16_t service_id = GetParam();
    // auto mock_socket = std::make_unique<MockSocket>();
    // EXPECT_CALL(*mock_socket, Transmit(::testing::_, ::testing::_, ::testing::_))
    //   .WillOnce(::testing::Return(simba::core::ErrorCode::kOk));
    // wrapper.TestInit(service_id, std::move(mock_socket));
    // wrapper.TestSubscribe();
    // EXPECT_EQ(wrapper.TestSubscribe(), simba::core::ErrorCode::kOk);

}