/**
 * @file csvdriver.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Driver that operates on a logger service output file
 * @version 0.1
 * @date 2024-06-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_CSVDRIVER_CSVDRIVER_H_
#define CORE_CSVDRIVER_CSVDRIVER_H_
#include <fstream>
#include <string>
#include <memory>
#include "core/csvdriver/Icsvdriver.h"
#include "core/file/file.hpp"
namespace srp {
namespace csv {
class CSVDriver : public ICSVDriver {
 private:
  std::ofstream csvFile;
  const char separator_;
  std::unique_ptr<core::IFileHandler> file_;
 public:
  void Init(std::unique_ptr<core::IFileHandler> handler_);
  explicit CSVDriver(const char& separator = ';');
  int Open(const std::string& fileName, const std::string& HEADER) override;
  void Close() override;
  int WriteLine(const std::string& line) override;
};
}  // namespace csv
}  // namespace srp
#endif  // CORE_CSVDRIVER_CSVDRIVER_H_
