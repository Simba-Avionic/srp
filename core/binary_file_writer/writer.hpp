/**
 * @file writer.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-02-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef CORE_BINARY_FILE_WRITER_WRITER_HPP_
#define CORE_BINARY_FILE_WRITER_WRITER_HPP_

#include <string>
#include <optional>
#include <fstream>
#include <vector>
#include <cstdint>

namespace srp {
namespace core {
namespace binaryWriter {

class BinaryFileWriter {
 private:
    std::fstream file_;
 public:
    ~BinaryFileWriter();
    bool open(const std::string& path);
    void close();
    bool write(const std::vector<uint8_t>& data, const bool flush_after_write = true);
};
}  // namespace binaryWriter
}  // namespace core
}  // namespace srp


#endif  // CORE_BINARY_FILE_WRITER_WRITER_HPP_
