/**
 * @file csvdriver.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/csvdriver/csvdriver.h"
#include <string>
#include <utility>
namespace srp {
namespace csv {

void CSVDriver::Init(std::unique_ptr<core::IFileHandler> handler_) {
    file_ = std::move(handler_);
}

CSVDriver::CSVDriver(const char& separator): separator_(separator) {
}
void CSVDriver::Close() {
    file_->close();
}
int CSVDriver::Open(const std::string& fileName, const std::string& HEADER) {
    if (!file_->open(fileName, core::FileMode::WRITE)) {
        return -1;
    }
    if (!file_->write(HEADER + "\n")) {
        return -1;
    }
    return 0;
}

int CSVDriver::WriteLine(const std::string& line) {
  if (!file_->write(line + "\n")) {
    return -1;
  }
  return 0;
}

}  // namespace csv
}  // namespace srp
