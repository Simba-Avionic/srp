/**
 * @file csv_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/csvdriver/csvdriver.h"
#include "core/file/mock_file.hpp"

using ::testing::Return;
using ::testing::_;

class CSVDriverTest : public ::testing::Test {
 protected:
    std::unique_ptr<MockFileHandler> mockFileHandler;
    srp::csv::CSVDriver csvDriver{','};

    void SetUp() override {
        mockFileHandler = std::make_unique<MockFileHandler>();
    }
};

TEST_F(CSVDriverTest, Open_Success) {
    EXPECT_CALL(*mockFileHandler, open("test.csv", srp::core::FileMode::WRITE))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockFileHandler, write("HEADER\n", true))
        .WillOnce(Return(true));
    csvDriver.Init(std::move(mockFileHandler));
    EXPECT_EQ(csvDriver.Open("test.csv", "HEADER"), 0);
}

TEST_F(CSVDriverTest, Open_FailureOnOpen) {
    EXPECT_CALL(*mockFileHandler, open(_, _)).WillOnce(Return(false));
    csvDriver.Init(std::move(mockFileHandler));
    EXPECT_EQ(csvDriver.Open("test.csv", "HEADER"), -1);
}

TEST_F(CSVDriverTest, Open_FailureOnWriteHeader) {
    EXPECT_CALL(*mockFileHandler, open(_, _)).WillOnce(Return(true));
    EXPECT_CALL(*mockFileHandler, write(_, _)).WillOnce(Return(false));
    csvDriver.Init(std::move(mockFileHandler));
    EXPECT_EQ(csvDriver.Open("test.csv", "HEADER"), -1);
}

TEST_F(CSVDriverTest, WriteLine_Success) {
    EXPECT_CALL(*mockFileHandler, write("data\n", true)).WillOnce(Return(true));
    csvDriver.Init(std::move(mockFileHandler));
    EXPECT_EQ(csvDriver.WriteLine("data"), 0);
}

TEST_F(CSVDriverTest, WriteLine_Failure) {
    EXPECT_CALL(*mockFileHandler, write("data\n", true)).WillOnce(Return(false));
    csvDriver.Init(std::move(mockFileHandler));
    EXPECT_EQ(csvDriver.WriteLine("data"), -1);
}

TEST_F(CSVDriverTest, Close) {
    EXPECT_CALL(*mockFileHandler, close()).Times(1);
    csvDriver.Init(std::move(mockFileHandler));
    csvDriver.Close();
}
