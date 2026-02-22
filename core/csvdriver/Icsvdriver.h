/**
 * @file Icsvdriver.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Interface for csvdriver
 * @version 0.1
 * @date 2024-08-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_CSVDRIVER_ICSVDRIVER_H_
#define CORE_CSVDRIVER_ICSVDRIVER_H_
#include <string>
#include <memory>
#include "core/file/file.hpp"
namespace srp {
namespace csv {

class ICSVDriver {
 public:
  virtual void Init(std::unique_ptr<core::IFileHandler> handler_ = std::make_unique<core::FileHandler>()) = 0;
  virtual int WriteLine(const std::string& line, const bool flush_after_write = true) = 0;
  virtual int Open(const std::string& fileName, const std::string& HEADER) = 0;
  virtual void Close() = 0;
};
}  // namespace csv
}  // namespace srp
#endif  // CORE_CSVDRIVER_ICSVDRIVER_H_
