/**
 * @file fields.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef ARA_COM_PROXY_FIELDS_H_
#define ARA_COM_PROXY_FIELDS_H_

#include "ara/com/types.h"
#include "ara/core/instance_specifier.h"

namespace ara {
namespace com {
namespace proxy {
template <typename T>
class Fields {
 private:
  /* data */
 public:
  explicit Fields(const ara::core::InstanceSpecifier& instance) noexcept {}

  explicit Fields(Fields&&) = delete;
  explicit Fields(Fields&) = delete;
  Fields operator=(Fields&) = delete;
  Fields operator=(Fields&&) = delete;

  ~Fields() noexcept {}

  void Unsubscribe() noexcept {}

  ara::core::Result<void> Subscribe(
      ara::com::SubscriptionStateChangeHandler&& handler,
      size_t maxSampleCount) noexcept {}

  ara::com::SubscriptionState GetSubscriptionState() const {}

  ara::core::Result<void> SetReceiveHandler(
      ara::com::FieldsReceiveHandler&& handler) noexcept {}

  ara::core::Result<size_t> GetNewSamples(GetNewSampleHandler<T>&& handler,
                                          size_t maxNumberOfSamples) noexcept {}

  ara::core::Result<T> Get() const noexcept {}
  ara::core::Result<void> Set(const T& val) const noexcept {}
};
}  // namespace proxy
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_PROXY_FIELDS_H_
