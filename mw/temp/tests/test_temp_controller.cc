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

#include <gtest/gtest.h>

#include <memory>

#include "mw/temp/controller/temp_controller.h"
#include "communication-core/sockets/mock/mockSocket.h"

class TestWrapper : public simba::mw::temp::TempController {
 public:
  simba::core::ErrorCode TestInit(uint16_t service_id, std::unique_ptr<MockSocket> socket) {
      return Init(service_id, std::move(socket));
  }
  simba::core::ErrorCode TestSubscribe() {
    return this->Subscribe();
  }
  simba::core::ErrorCode TestSetUp(simba::com::soc::RXCallback callback) {
    return this->SetUp(callback);
  }
};


TEST(TEMP_CONTROLLER, TEMP_SUBSCRIBE_TEST) {
    TestWrapper wrapper{};
    auto mock_socket = std::make_unique<MockSocket>();
    EXPECT_CALL(*mock_socket, Transmit(::testing::_, ::testing::_, ::testing::_))
      .WillOnce(::testing::Return(simba::core::ErrorCode::kOk))
      .WillOnce(::testing::Return(simba::core::ErrorCode::kError));
    wrapper.TestInit(0, std::move(mock_socket));
    EXPECT_EQ(wrapper.TestSubscribe(), simba::core::ErrorCode::kOk);
    EXPECT_EQ(wrapper.TestSubscribe(), simba::core::ErrorCode::kError);
}


TEST(TEMP_CONTROLLER, TEMP_SETUP_TEST) {
  TestWrapper wrapper{};
    auto mock_socket = std::make_unique<MockSocket>();
    EXPECT_CALL(*mock_socket, Transmit(::testing::_, ::testing::_, ::testing::_))
      .WillOnce(::testing::Return(simba::core::ErrorCode::kOk))
      .WillOnce(::testing::Return(simba::core::ErrorCode::kError));
    EXPECT_CALL(*mock_socket, Init(::testing::_))
      .WillOnce(::testing::Return(simba::core::ErrorCode::kOk))
      .WillOnce(::testing::Return(simba::core::ErrorCode::kOk))
      .WillOnce(::testing::Return(simba::core::ErrorCode::kInitializeError));
    EXPECT_CALL(*mock_socket, SetRXCallback(::testing::_))
      .WillOnce(::testing::Return())
      .WillOnce(::testing::Return());
    EXPECT_CALL(*mock_socket, StartRXThread())
      .WillOnce(::testing::Return());
    wrapper.TestInit(0, std::move(mock_socket));
    auto callback = [](const std::string& ip,
      const std::uint16_t& port, const std::vector<std::uint8_t> data){};
    EXPECT_EQ(wrapper.TestSetUp(callback), simba::core::ErrorCode::kOk);
    EXPECT_EQ(wrapper.TestSetUp(callback), simba::core::ErrorCode::kError);
    EXPECT_EQ(wrapper.TestSetUp(callback), simba::core::ErrorCode::kInitializeError);
}
