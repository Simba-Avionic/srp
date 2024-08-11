/**
 * @file exception.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_CORE_EXCEPTION_H_
#define ARA_CORE_EXCEPTION_H_
#include <exception>

#include "ara/core/error_code.h"
namespace ara {
namespace core {
class Exception : public std::exception {
 protected:
  ErrorCode err_;

 public:
  explicit Exception(ErrorCode err) noexcept : err_{err} {}
  Exception(Exception &&other) = default;
  Exception(const Exception &other) = default;

  Exception &operator=(Exception &&other) & = default;
  Exception &operator=(const Exception const &other) = default;

  virtual ~Exception() = default;

  const char *what() const noexcept override { return err_.Message().c_str(); }
  const ErrorCode &Error() const noexcept { return err_; }
};
}  // namespace core
}  // namespace ara
#endif  // ARA_CORE_EXCEPTION_H_
