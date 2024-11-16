/**
 * @file result.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_CORE_RESULT_H_
#define ARA_CORE_RESULT_H_

#include <variant>

#include "ara/core/error_code.h"

namespace ara {
namespace core {
template <typename T, typename E = ErrorCode>
class Result final {
 private:
  const std::variant<T, E> value_{};

 public:
  using value_type = T;
  using error_type = E;

  Result(const T &t) : value_{t} {}
  Result(T &&t) : value_{t} {}
  Result(const E &e) : value_{e} {}
  Result(E &&e) : value_{e} {}
  Result(const Result<T, E> &other) : value_{other.value_} {}

  ~Result() noexcept = default;

  static Result FromValue(const T &t) { return Result{t}; }
  static Result FromValue(T &&t) { return Result{t}; }
  static Result FromError(const E &e) { return Result{e}; }
  static Result FromError(const E &&e) { return Result{e}; }
  bool HasValue() const noexcept { return (value_.index() == 0); }
  explicit operator bool() const noexcept { return (value_.index() == 0); }
  const T &operator*() const { return std::get<T>(value_); }
  T &&operator*() { return std::get<T>(value_); }
  const T *operator->() const { return &std::get<T>(value_); }
  const T &Value() const { return std::get<T>(value_); }
  //  T &&Value() { return std::get<T>(value_); }
  const E &Error() const { return std::get<E>(value_); }
  template <typename U>
  T ValueOr(U &&defaultValue) const {
    if (HasValue()) {
      return std::get<T>(value_);
    } else {
      return defaultValue;
    }
  }
  template <typename G>
  E ErrorOr(G &&defaultError) const {
    if (!HasValue()) {
      return std::get<E>(value_);
    } else {
      return defaultError;
    }
  }

  template <typename G>
  bool CheckError(G &&error) const {
    if (!HasValue()) {
      return std::get<E>(value_) == error;
    } else {
      return false;
    }
  }
};

template <>
class Result<void, ErrorCode> {
 private:
  const std::variant<std::monostate, ErrorCode> value_{};

 public:
  using error_type = ErrorCode;

  Result() : value_{} {}
  Result(const ErrorCode &e) : value_{e} {}  // NOLINT
  Result(ErrorCode &&e) : value_{e} {}       // NOLINT
  Result(const Result<void, ErrorCode> &other) : value_{other.value_} {}

  ~Result() noexcept = default;

  static Result FromValue() { return Result{}; }
  static Result FromError(const ErrorCode &e) { return Result{e}; }
  static Result FromError(const ErrorCode &&e) { return Result{e}; }
  bool HasValue() const noexcept { return (value_.index() == 0); }
  explicit operator bool() const noexcept { return (value_.index() == 0); }
  const void operator*() const {}
  const void operator->() const {}
  void Value() const {}

  const ErrorCode &Error() const { return std::get<ErrorCode>(value_); }

  template <typename G>
  ErrorCode ErrorOr(G &&defaultError) const {
    if (!HasValue()) {
      return std::get<ErrorCode>(value_);
    } else {
      return defaultError;
    }
  }

  template <typename G>
  bool CheckError(G &&error) const {
    if (!HasValue()) {
      return std::get<ErrorCode>(value_) == error;
    } else {
      return false;
    }
  }
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_RESULT_H_
