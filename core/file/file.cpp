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

namespace {
const auto ModeToString(const FileMode& mode) {
    return (mode == FileMode::READ) ? "READ" : "WRITE";
}
}  // namespace

FileHandler::FileHandler():
    logger_(ara::log::LoggingMenager::GetInstance()->CreateLogger(
        "file", "", ara::log::LogLevel::kWarn)) {}

FileHandler::~FileHandler() {
    this->close();
}

bool FileHandler::open(const std::string& path, const FileMode& mode) {
    if (file_.is_open()) {
        logger_.LogDebug() << "Reopening file, closing previous path: " << current_path_;
        file_.close();
    }
    current_path_ = path;
    std::ios_base::openmode open_mode = (mode == FileMode::READ) ? std::ios::in : std::ios::out;
    file_.open(path, open_mode);
    if (file_.is_open()) {
        logger_.LogDebug() << "Opened file: " << path << ", mode: " << ModeToString(mode);
        return true;
    }
    logger_.LogWarn() << "Failed to open file: " << path << ", mode: " << ModeToString(mode);
    return false;
}

void FileHandler::close() {
    if (file_.is_open()) {
        logger_.LogDebug() << "Closing file: " << current_path_;
        file_.close();
    }
}

bool FileHandler::write(const std::string& data, const bool flush_after_write) {
    if (!file_.is_open()) {
        logger_.LogWarn() << "Write failed: file is not open"
                          << (current_path_.empty() ? "" : ", last path: ") << current_path_;
        return false;
    }
    file_ << data;
    if (!file_) {
        logger_.LogWarn() << "Write failed for file: " << current_path_
                          << ", size: " << data.size();
        return false;
    }
    if (flush_after_write) {
        file_.flush();
        if (!file_) {
            logger_.LogWarn() << "Flush failed for file: " << current_path_;
            return false;
        }
    }
    logger_.LogDebug() << "Wrote " << data.size() << " bytes to file: " << current_path_;
    return true;
}

std::optional<std::string> FileHandler::read() {
    if (!file_.is_open()) {
        logger_.LogWarn() << "Read failed: file is not open"
                          << (current_path_.empty() ? "" : ", last path: ") << current_path_;
        return std::nullopt;
    }
    std::string data;
    std::getline(file_, data);
    if (file_.eof() && data.empty()) {
        logger_.LogDebug() << "Read EOF from file: " << current_path_;
        return std::nullopt;
    }
    logger_.LogDebug() << "Read " << data.size() << " bytes from file: " << current_path_;
    return data;
}
}  // namespace core
}  // namespace srp
