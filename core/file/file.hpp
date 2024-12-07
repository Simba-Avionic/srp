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
namespace simba {
namespace core {
enum File_mode_t {
  READ,
  WRITE,
};
class FileHandler {
 private:
    std::fstream file_;
 public:
    ~FileHandler();
    FileHandler();
    bool open(const std::string& path, const File_mode_t& mode);
    void close();
    bool write(const std::string& data);
    std::optional<std::string> read();
};
}  // namespace core
}  // namespace simba

#endif  // CORE_FILE_FILE_HPP_
