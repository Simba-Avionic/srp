/**
 * @file event.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef ARA_COM_PROXY_EVENT_H_
#define ARA_COM_PROXY_EVENT_H_

#include "ara/com/types.h"
#include "ara/core/instance_specifier.h"

namespace ara {
namespace com {
namespace proxy {
template <typename T>
class Event {
 private:
  /* data */
 public:
  explicit Event(const ara::core::InstanceSpecifier& instance) noexcept {}

  explicit Event(Event&&) = delete;
  explicit Event(Event&) = delete;
  Event operator=(Event&) = delete;
  Event operator=(Event&&) = delete;

  ~Event() noexcept {}

  void Unsubscribe() noexcept {}

  ara::core::Result<void> Subscribe(
      ara::com::SubscriptionStateChangeHandler&& handler,
      size_t maxSampleCount) noexcept {}

  ara::com::SubscriptionState GetSubscriptionState() const {}

  ara::core::Result<void> SetReceiveHandler(
      ara::com::EventReceiveHandler&& handler) noexcept {}

  ara::core::Result<size_t> GetNewSamples(GetNewSampleHandler<T>&& handler,
                                          size_t maxNumberOfSamples) noexcept {}
};
}  // namespace proxy
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_PROXY_EVENT_H_
