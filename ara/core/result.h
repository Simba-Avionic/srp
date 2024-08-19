#ifndef ARA_CORE_RESULT_H_
#define ARA_CORE_RESULT_H_

#include <optional>

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

  Result(const T &t) : value_{t}, error_{std::nullopt} {}
  Result(T &&t) : value_{t}, error_{std::nullopt} {}
  Result(const E &e) : value_{std::nullopt}, error_{e} {}
  Result(E &&e) : value_{std::nullopt}, error_{e} {}
  Result(const Result<T, E> &other)
      : value_{other.value_}, error_{other.error_} {}

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
  T &&Value() { return value_.value(); }
  const E &Error() const { return error_.value(); }
  template <typename U>
  T ValueOr(U &&defaultValue) const {
    return value_.value_or(defaultValue);
  }
  template <typename G>
  E ErrorOr(G &&defaultError) const {
    return error_.value_or(defaultError);
  }

  template <typename G>
bool CheckError (G &&error) const {
    if(error_.has_value()){
        return error_.value() == error;
    } else {
        return false;
    }
}
};
}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_RESULT_H_
