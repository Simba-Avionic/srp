/**
 * @file writer.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-02-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "core/binary_file_writer/writer.hpp"
#include <cstdint>
#include <vector>

namespace srp {
namespace core {
namespace binaryWriter {

BinaryFileWriter::~BinaryFileWriter() {
    close();
}
bool BinaryFileWriter::open(const std::string& path) {
    std::ios_base::openmode open_mode = std::ios::out | std::ios::binary;
    file_.open(path, open_mode);
    return file_.is_open();
}
void BinaryFileWriter::close() {
    if (file_.is_open()) {
        file_.close();
    }
}
bool BinaryFileWriter::write(const std::vector<uint8_t>& data, const bool flush_after_write) {
    if (data.empty()) {
        return false;
    }
    if (!file_.is_open()) {
        return false;
    }
    file_.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!file_) {
        return false;
    }
    if (flush_after_write) {
        file_.flush();
    }
    return true;
}

}  // namespace binaryWriter
}  // namespace core
}  // namespace srp
