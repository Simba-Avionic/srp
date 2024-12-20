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
#include "core/file/file.hpp"
class MockFileHandler : public simba::core::FileHandler {
 public:
    MOCK_METHOD(bool, open, (const std::string& path, const simba::core::File_mode_t& mode), ());
    MOCK_METHOD(void, close, (), ());
    MOCK_METHOD(bool, write, (const std::string& data), ());
    MOCK_METHOD(std::optional<std::string>, read, (), ());
};

#endif  // CORE_FILE_MOCK_FILE_HPP_

