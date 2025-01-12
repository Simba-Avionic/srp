/**
 * @file Method.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef ARA_COM_PROXY_METHOD_H_
#define ARA_COM_PROXY_METHOD_H_

#include "ara/com/types.h"
#include "ara/core/instance_specifier.h"

namespace ara {
namespace com {
namespace proxy {
template <typename T_OUT, typename T_IN>
class Method {
 private:
  /* data */
 public:
  explicit Method(const ara::core::InstanceSpecifier& instance) noexcept {}

  explicit Method(Method&&) = delete;
  explicit Method(Method&) = delete;
  Method operator=(Method&) = delete;
  Method operator=(Method&&) = delete;

  ~Method() noexcept {}

  ara::core::Result<T_OUT> operator()(const T_IN& val) {}
};
}  // namespace proxy
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_PROXY_METHOD_H_
