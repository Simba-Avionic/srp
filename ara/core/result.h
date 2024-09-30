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

#include <optional>
#include <variant>

#include "ara/core/error_code.h"

namespace ara {
namespace core {
template <typename T, typename E = ErrorCode>
class Result final {
 private:
  const std::optional<T> value_{};
  const std::optional<E> error_{};

 public:
  using value_type = T;
  using error_type = E;

  Result(const T &t) : value_{t} {}
  Result(T &&t) : value_{t} {}
  Result(const E &e) : error_{e} {}
  Result(E &&e) : error_{e} {}
  Result(const Result<T, E> &other)
      : value_{other.value_}, error_(other.error_) {}

  ~Result() noexcept = default;

  static Result FromValue(const T &t) { return Result{t}; }
  static Result FromValue(T &&t) { return Result{t}; }
  static Result FromError(const E &e) { return Result{e}; }
  static Result FromError(const E &&e) { return Result{e}; }
  bool HasValue() const noexcept { return value_.has_value(); }
  explicit operator bool() const noexcept { return value_.has_value(); }
  const T &operator*() const { return value_.value(); }
  T &&operator*() { return value_.value(); }
  const T *operator->() const { return &value_.value(); }
  const T &Value() const { return value_.value(); }
  // T &&Value() { return value_.value(); }
  const E &Error() const { return error_.value(); }
  template <typename U>
  T ValueOr(U &&defaultValue) const {
    if (HasValue()) {
      return value_.value();
    } else {
      return defaultValue;
    }
  }
  template <typename G>
  E ErrorOr(G &&defaultError) const {
    if (!HasValue()) {
      return error_.value();
    } else {
      return defaultError;
    }
  }

  template <typename G>
  bool CheckError(G &&error) const {
    if (!HasValue()) {
      return error_.value() == error;
    } else {
      return false;
    }
  }
};

template <>
class Result<void, ErrorCode> {
 private:
  const std::optional<ErrorCode> error_{};

 public:
  using error_type = ErrorCode;

  Result() : error_{std::nullopt} {}
  Result(const ErrorCode &e) : error_{e} {}  // NOLINT
  Result(ErrorCode &&e) : error_{e} {}       // NOLINT
  Result(const Result<void, ErrorCode> &other) : error_{other.error_} {}

  ~Result() noexcept = default;

  static Result FromValue() { return Result{}; }
  static Result FromError(const ErrorCode &e) { return Result{e}; }
  static Result FromError(const ErrorCode &&e) { return Result{e}; }
  bool HasValue() const noexcept { return !error_.has_value(); }
  explicit operator bool() const noexcept { return !error_.has_value(); }
  const void operator*() const {}
  const void operator->() const {}
  void Value() const {}

  const ErrorCode &Error() const { return error_.value(); }

  template <typename G>
  ErrorCode ErrorOr(G &&defaultError) const {
    if (!HasValue()) {
      return error_.value();
    } else {
      return defaultError;
    }
  }

  template <typename G>
  bool CheckError(G &&error) const {
    if (!HasValue()) {
      return error_.value() == error;
    } else {
      return false;
    }
  }
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_RESULT_H_
