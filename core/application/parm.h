/**
 * @file parm.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Application paramiters
 * @version 1.0
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_APPLICATION_PARM_H_
#define CORE_APPLICATION_PARM_H_
#include <algorithm>
#include <string>
namespace simba {
namespace core {
class Parm {
 protected:
  std::string name{""};
  std::string value{""};

 public:
  explicit Parm(const std::string raw) {
    if (raw.find("=") != std::string::npos) {
      name = raw.substr(0, raw.find("="));
      value = raw.substr(raw.find("=")+1);
    } else {
      name = raw;
    }
  }
  const std::string GetValue() const { return value; }
  const std::string GetName() const { return name; }
  ~Parm() = default;
};
}  // namespace core
}  // namespace simba
#endif  // CORE_APPLICATION_PARM_H_
