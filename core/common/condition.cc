/**
 * @file condition.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/common/condition.h"

namespace srp {
namespace core {
namespace condition {

void wait(const std::stop_token& stoken) {
  std::condition_variable_any cv;
  std::mutex mutex_;
  std::unique_lock<std::mutex> ul_{mutex_};
  const std::stop_callback stop_wait{stoken, [&cv]() { cv.notify_one(); }};
  cv.wait(ul_, [&stoken]() { return stoken.stop_requested(); });
}

}  // namespace condition
}  // namespace core
}  // namespace srp
