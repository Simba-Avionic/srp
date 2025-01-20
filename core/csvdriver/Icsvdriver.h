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
namespace srp {
namespace csv {

class ICSVDriver {
 public:
  virtual int WriteLine(const std::string& line) = 0;
  virtual int Open(const std::string& fileName, const std::string& HEADER) = 0;
  virtual void Close() = 0;
};
}  // namespace csv
}  // namespace srp
#endif  // CORE_CSVDRIVER_ICSVDRIVER_H_
