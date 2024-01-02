/**
 * @file result.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CORE_RESULTS_RESULT_H_
#define CORE_RESULTS_RESULT_H_
#include <cstdbool>
#include <memory>

namespace simba {
namespace core {
template <typename T>
class Result {
 private:
  std::shared_ptr<T> value;
  bool was_set{false};

 public:
  Result() { value = nullptr; }
  explicit Result(const T& value) {
    this->value = std::make_shared<T>(value);
    this->was_set = true;
  }
  bool HasValue() const { return was_set; }
  T Value() const { return *this->value.get(); }
  void SetValue(const T& value) {
    this->value = value;
    this->was_set = true;
  }
};
}  // namespace core
}  // namespace simba
#endif  // CORE_RESULTS_RESULT_H_
