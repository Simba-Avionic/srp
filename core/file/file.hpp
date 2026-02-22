/**
 * @file file.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CORE_FILE_FILE_HPP_
#define CORE_FILE_FILE_HPP_
#include <string>
#include <optional>
#include <fstream>
#include "core/file/Ifile.hpp"
namespace srp {
namespace core {
class FileHandler: public IFileHandler {
 private:
    std::fstream file_;
 public:
    ~FileHandler() override;
    FileHandler();
    bool open(const std::string& path, const FileMode& mode) override;
    void close() override;
    bool write(const std::string& data, const bool flush_after_write = true) override;
    std::optional<std::string> read() override;
};
}  // namespace core
}  // namespace srp

#endif  // CORE_FILE_FILE_HPP_

