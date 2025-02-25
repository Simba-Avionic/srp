/**
 * @file file_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "core/file/file.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class FileHandlerTest : public ::testing::Test {
 protected:
    srp::core::FileHandler fileHandler;
    std::string testFilePath = "test_file.txt";

    void TearDown() override {
        if (fs::exists(testFilePath)) {
            fs::remove(testFilePath);
        }
    }
};

TEST_F(FileHandlerTest, OpenFileSuccess) {
    EXPECT_TRUE(fileHandler.open(testFilePath, srp::core::FileMode::WRITE));
    fileHandler.close();
}

TEST_F(FileHandlerTest, OpenFileFailure) {
    std::string invalidPath = "/invalid_path/test_file.txt";
    EXPECT_FALSE(fileHandler.open(invalidPath, srp::core::FileMode::WRITE));
}

TEST_F(FileHandlerTest, WriteToFileSuccess) {
    fileHandler.open(testFilePath, srp::core::FileMode::WRITE);
    EXPECT_TRUE(fileHandler.write("Test data"));
    fileHandler.close();

    std::ifstream file(testFilePath);
    std::string content;
    std::getline(file, content);
    EXPECT_EQ(content, "Test data");
}

TEST_F(FileHandlerTest, WriteToFileFailureWhenClosed) {
    EXPECT_FALSE(fileHandler.write("Data"));
}

TEST_F(FileHandlerTest, ReadFromFileSuccess) {
    std::ofstream file(testFilePath);
    file << "Test data\n";
    file.close();

    fileHandler.open(testFilePath, srp::core::FileMode::READ);
    auto result = fileHandler.read();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "Test data");
}

TEST_F(FileHandlerTest, ReadFromFileFailureWhenClosed) {
    auto result = fileHandler.read();
    EXPECT_FALSE(result.has_value());
}

TEST_F(FileHandlerTest, ReadFromFileEndOfFile) {
    std::ofstream file(testFilePath);
    file.close();

    fileHandler.open(testFilePath, srp::core::FileMode::READ);
    auto result = fileHandler.read();
    EXPECT_FALSE(result.has_value());
}
