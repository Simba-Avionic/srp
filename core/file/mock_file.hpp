/**
 * @file mock_file.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_FILE_MOCK_FILE_HPP_
#define CORE_FILE_MOCK_FILE_HPP_
#include <string>
#include "gmock/gmock.h"
#include "core/file/Ifile.hpp"
class MockFileHandler : public srp::core::IFileHandler {
 public:
    MOCK_METHOD(bool, open, (const std::string& path, const srp::core::FileMode& mode), (override));
    MOCK_METHOD(void, close, (), (override));
    MOCK_METHOD(bool, write, (const std::string& data), (override));
    MOCK_METHOD(std::optional<std::string>, read, (), (override));
};

#endif  // CORE_FILE_MOCK_FILE_HPP_

