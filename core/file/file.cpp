/**
 * @file file.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/file/file.hpp"

namespace srp {
namespace core {

FileHandler::FileHandler() {
}
FileHandler::~FileHandler() {
    this->close();
}

bool FileHandler::open(const std::string& path, const FileMode& mode) {
    std::ios_base::openmode open_mode = (mode == FileMode::READ) ? std::ios::in : std::ios::out;
    file_.open(path, open_mode);
    if (file_.is_open()) {
        return true;
    } else {
        return false;
    }
}

void FileHandler::close() {
    if (file_.is_open()) {
        file_.close();
    }
}

bool FileHandler::write(const std::string& data, const bool flush_after_write) {
    if (!file_.is_open()) {
        return false;
    }
    file_ << data;
    if (!file_) {  // Sprawdzenie błędu zapisu
        return false;
    }
    if (flush_after_write) {
        file_.flush();
    }
    return true;
}

std::optional<std::string> FileHandler::read() {
    if (!file_.is_open()) {
        return std::nullopt;
    }
    std::string data;
    std::getline(file_, data);
    if (file_.eof() && data.empty()) {
        return std::nullopt;
    }
    return data;
}
}  // namespace core
}  // namespace srp
