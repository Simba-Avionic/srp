/**
 * @file condition.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CORE_COMMON_CONDITION_H_
#define CORE_COMMON_CONDITION_H_
#include <chrono>              // NOLINT
#include <condition_variable>  // NOLINT
#include <stop_token>          // NOLINT

namespace srp {
namespace core {
namespace condition {
template <typename _Rep, typename _Period>
void wait_for(const std::chrono::duration<_Rep, _Period>& dur,
              const std::stop_token& stoken) {
  std::condition_variable_any cv;
  std::mutex mutex_;
  std::unique_lock<std::mutex> ul_{mutex_};
  const std::stop_callback stop_wait{stoken, [&cv]() { cv.notify_one(); }};
  cv.wait_for(ul_, dur, [&stoken]() { return stoken.stop_requested(); });
}

void wait(const std::stop_token& stoken);

}  // namespace condition
}  // namespace core
}  // namespace srp

#endif  // CORE_COMMON_CONDITION_H_
