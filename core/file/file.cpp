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
#include "ara/log/log.h"

namespace srp {
namespace core {

FileHandler::FileHandler() {
}
FileHandler::~FileHandler() {
    try {
        if (file_.is_open()) {
            file_.close();
            ara::log::LogDebug() << "File closed successfully in destructor.";
        }
    } catch (const std::ios_base::failure& e) {
        ara::log::LogError() << "Error closing file in destructor: " << e.what();
    }
}

bool FileHandler::open(const std::string& path, const File_mode_t& mode) {
    try {
        std::ios_base::openmode open_mode = (mode == File_mode_t::READ) ? std::ios::in : std::ios::out;
        file_.open(path, open_mode);
        if (file_.is_open()) {
            ara::log::LogDebug() << "Opened file: " << path;
            return true;
        } else {
            ara::log::LogError() << "Failed to open file: " << path;
            return false;
        }
    } catch (const std::ios_base::failure& e) {
        ara::log::LogError() << "Exception opening file: " << e.what();
        return false;
    }
}

void FileHandler::close() {
    try {
        if (file_.is_open()) {
            file_.close();
            ara::log::LogDebug() << "Closed file successfully.";
        } else {
            ara::log::LogWarn() << "Close called, but no file was open.";
        }
    } catch (const std::ios_base::failure& e) {
        ara::log::LogError() << "Exception closing file: " << e.what();
    }
}

bool FileHandler::write(const std::string& data) {
    if (!file_.is_open()) {
        ara::log::LogError() << "Cannot write to file: file is not open.";
        return false;
    }
    try {
        file_ << data;
        if (!file_) {  // Sprawdzenie błędu zapisu
            ara::log::LogError() << "Failed to write data to file.";
            return false;
        }
        ara::log::LogDebug() << "Wrote data to file: " << data;
        return true;
    } catch (const std::ios_base::failure& e) {
        ara::log::LogError() << "Exception writing to file: " << e.what();
        return false;
    }
}

std::optional<std::string> FileHandler::read() {
    if (!file_.is_open()) {
        ara::log::LogError() << "Cannot read from file: file is not open.";
        return std::nullopt;
    }
    try {
        std::string data;
        std::getline(file_, data);
        if (file_.eof() && data.empty()) {
            ara::log::LogWarn() << "End of file reached.";
            return std::nullopt;
        }
        ara::log::LogDebug() << "Read data: " << data;
        return data;
    } catch (const std::ios_base::failure& e) {
        ara::log::LogError() << "Exception reading from file: " << e.what();
        return std::nullopt;
    }
}
}  // namespace core
}  // namespace srp
