/**
 * @file test_i2ccontroller.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mw/i2c_service/controller/i2c_controller.h"
#include "communication-core/sockets/mock/mockSocket.h"  // Mock klasy StreamIpcSocket

using ::testing::_;
using ::testing::Return;

namespace simba {
namespace i2c {

class I2CControllerTest : public ::testing::Test {
 protected:
    void SetUp() override {
        mock_socket_ = std::make_unique<testing::NiceMock<MockStreamSocket>>();
    }

    I2CController controller_;
    std::unique_ptr<testing::NiceMock<MockStreamSocket>> mock_socket_;
};

TEST_F(I2CControllerTest, InitSuccess) {
    EXPECT_EQ(controller_.Init(nullptr), core::ErrorCode::kOk);
}

class WriteTest : public I2CControllerTest,
                  public ::testing::WithParamInterface<std::tuple<uint8_t, std::vector<uint8_t>, bool>> {};

TEST_P(WriteTest, Write) {
    uint8_t address = std::get<0>(GetParam());
    std::vector<uint8_t> data = std::get<1>(GetParam());
    bool transmission_success = std::get<2>(GetParam());

    // Oczekiwane wywołanie metody Transmit
    EXPECT_CALL(*mock_socket_, Transmit(_, _, _))
        .WillOnce(Return(transmission_success ? std::make_optional<std::vector<uint8_t>>({1}) : std::nullopt));

    controller_.Init(std::move(mock_socket_));

    auto result = controller_.Write(address, data);
    if (transmission_success) {
        EXPECT_EQ(result, core::ErrorCode::kOk);
    } else {
        EXPECT_EQ(result, core::ErrorCode::kConnectionError);
    }
}

INSTANTIATE_TEST_SUITE_P(
    WriteTestCases,
    WriteTest,
    ::testing::Values(
        std::make_tuple(0x50, std::vector<uint8_t>{0x01, 0x02}, true),
        std::make_tuple(0x50, std::vector<uint8_t>{0x01, 0x02}, false)
));

TEST_F(I2CControllerTest, PageWriteSuccess) {
    uint8_t address = 0x50;
    std::vector<uint8_t> data = {0x01, 0x02};

    EXPECT_CALL(*mock_socket_, Transmit(_, _, _))
        .WillOnce(Return(std::make_optional<std::vector<uint8_t>>({1})));

    controller_.Init(std::move(mock_socket_));

    auto result = controller_.PageWrite(address, data);

    EXPECT_EQ(result, core::ErrorCode::kOk);
}
TEST_F(I2CControllerTest, PageWriteFail) {
    auto mock_socket_ = std::make_unique<testing::NiceMock<MockStreamSocket>>();
    I2CController controller_;
    uint8_t address = 0x50;
    std::vector<uint8_t> data = {0x01, 0x02};

    EXPECT_CALL(*mock_socket_, Transmit(_, _, _))
        .WillOnce(Return(std::nullopt));

    controller_.Init(std::move(mock_socket_));

    auto result = controller_.PageWrite(address, data);

    EXPECT_EQ(result, core::ErrorCode::kConnectionError);
}

TEST_F(I2CControllerTest, ReadSuccess) {
    uint8_t address = 0x50;
    uint8_t reg = 0x01;
    uint8_t size = 2;

    std::vector<uint8_t> expected_data = {0x03, 0x04};

    EXPECT_CALL(*mock_socket_, Transmit(_, _, _))
        .WillOnce(Return(expected_data));

    controller_.Init(std::move(mock_socket_));

    auto result = controller_.Read(address, reg, size);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), expected_data);
}

TEST_F(I2CControllerTest, WriteReadFailure) {
    uint8_t address = 0x50;
    uint8_t write_data = 0x01;
    uint8_t read_size = 2;

    EXPECT_CALL(*mock_socket_, Transmit(_, _, _))
        .WillOnce(Return(std::nullopt));

    controller_.Init(std::move(mock_socket_));

    auto result = controller_.WriteRead(address, write_data, read_size);
    EXPECT_FALSE(result.has_value());
}

}  // namespace i2c
}  // namespace simba
