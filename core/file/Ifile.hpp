/**
 * @file Ifile.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef CORE_FILE_IFILE_HPP_
#define CORE_FILE_IFILE_HPP_

#include <string>
#include <optional>

namespace srp {
namespace core {

enum class FileMode {
  READ,
  WRITE,
};

class IFileHandler {
 public:
    virtual ~IFileHandler() = default;
    virtual bool open(const std::string& path, const FileMode& mode) = 0;
    virtual void close() = 0;
    virtual bool write(const std::string& data) = 0;
    virtual std::optional<std::string> read() = 0;
};

}  // namespace core
}  // namespace srp

#endif  // CORE_FILE_IFILE_HPP_
